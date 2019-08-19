#pragma once

#include <mutex>
#include <condition_variable>

#include <Core/Misc/Defines/Common.hpp>

#include "RenderEngine/Renderer/Backend/CommandPacket/CommandPacket.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/ICommandBuffer/ICommandBuffer.hpp"

TRE_NS_START

template<typename T>
class FramebufferCommandBucket : public ICommandBuffer<FramebufferCommandBucket<T>, T>
{
public:
    typedef T Key;
    typedef ICommandBuffer<FramebufferCommandBucket<T>, T> BaseClass;

    struct FrameBufferPiriority
	{
        typedef uint8 Piroirty_t;

		FboID fbo_id = RenderSettings::DEFAULT_FRAMEBUFFER;
		Piroirty_t pirority = 0;
	};

    FramebufferCommandBucket();

    Key GenerateKey(FrameBufferPiriority fbo_piriority, typename RMI<ShaderProgram>::ID shaderID, typename RMI<VAO>::ID vaoID, typename RMI<Material>::ID matID) const;

    void DecodeKey(Key key, FrameBufferPiriority& fbo_piriority, typename RMI<ShaderProgram>::ID& shaderID, typename RMI<VAO>::ID& vaoID, typename RMI<Material>::ID& matID) const;

    void Submit();

    void Clear();

    bool SwapCmdBuffer();
public:
    uint32 m_StartLocation, m_SecondCurrent;
    StateHash m_LastStateHash;
    std::mutex mtx;
    std::condition_variable cv;
    bool m_IsReading;
    
    CONSTEXPR static uint8  READ_CMD_BUFFER  = 0;
    CONSTEXPR static uint8  WRITE_CMD_BUFFER = 1;
};

template<typename T>
using FramebufferCommandBuffer = FramebufferCommandBucket<T>;


template<typename T>
FramebufferCommandBucket<T>::FramebufferCommandBucket() : 
    BaseClass(&FramebufferCommandBucket<T>::Submit, 2),
    m_StartLocation(BaseClass::DEFAULT_MAX_ELEMENTS), 
    m_SecondCurrent(BaseClass::DEFAULT_MAX_ELEMENTS), 
    m_LastStateHash(RenderSettings::DEFAULT_STATE_HASH),
    m_MainFboID(RenderSettings::DEFAULT_FRAMEBUFFER),
    mtx(),
    cv(),
    m_IsReading(false)
{
    // cv.notify_one();
}

template<typename T>
typename FramebufferCommandBucket<T>::Key FramebufferCommandBucket<T>::GenerateKey(
        FrameBufferPiriority fbo_piriority, 
        typename RMI<ShaderProgram>::ID shaderID, 
        typename RMI<VAO>::ID vaoID, 
        typename RMI<Material>::ID matID) const
{
    Key key = 0;

    typename FrameBufferPiriority::Piroirty_t pirority = fbo_piriority.pirority;
    RMI<FBO>::Index fbo_id = ResourcesManager::GetGRM().GetResourceContainer<FBO>().CompressID(fbo_piriority.fbo_id);
    RMI<ShaderProgram>::Index sid = ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>().CompressID(shaderID);
    RMI<VAO>::Index vao_id  = ResourcesManager::GetGRM().GetResourceContainer<VAO>().CompressID(vaoID);
    RMI<Material>::Index mat_id = ResourcesManager::GetGRM().GetResourceContainer<Material>().CompressID(matID);

#if ENDIANNESS == LITTLE_ENDIAN
    key = 
        (Key(pirority) << (sizeof(mat_id) + sizeof(vao_id) + sizeof(sid) + sizeof(fbo_id)) * BITS_PER_BYTE) | 
        (Key(fbo_id)   << (sizeof(mat_id) + sizeof(vao_id) + sizeof(sid)) * BITS_PER_BYTE) | 
        (Key(sid)      << (sizeof(mat_id) + sizeof(vao_id)) * BITS_PER_BYTE) | 
        (Key(vao_id)   << (sizeof(mat_id) * BITS_PER_BYTE)) | 
        Key(mat_id);
#else
    key = (Key(vao_id) << (sizeof(vao_id) + sizeof(sid)) * BITS_PER_BYTE) | (Key(mat_id) << (sizeof(sid) * BITS_PER_BYTE)) | Key(sid);
#endif

    // printf("\033[1;31m* Encode : ShaderID = %d | vaoID = %d| matID = %d |-->Key = %lu\n", sid, vao_id, mat_id, key);
    return key;
}

template<typename T>
void FramebufferCommandBucket<T>::DecodeKey(Key key, FrameBufferPiriority& fbo_piriority, typename RMI<ShaderProgram>::ID& shaderID, typename RMI<VAO>::ID& vaoID, typename RMI<Material>::ID& matID) const
{
#if ENDIANNESS == LITTLE_ENDIAN
    
    fbo_piriority.pirority = FrameBufferPiriority::Piroirty_t(
        key >> (sizeof(RMI<VAO>::Index) + sizeof(RMI<Material>::Index) + sizeof(RMI<ShaderProgram>::Index) + sizeof(RMI<FBO>::Index)) * BITS_PER_BYTE);

    fbo_piriority.fbo_id = ResourcesManager::GetGRM().GetResourceContainer<FBO>().CompressID(
        RMI<FBO>::ID(key >> (sizeof(RMI<VAO>::Index) + sizeof(RMI<Material>::Index) + sizeof(RMI<ShaderProgram>::Index)) * BITS_PER_BYTE));

    shaderID = ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>().CompressID(
        RMI<ShaderProgram>::ID(key >> (sizeof(RMI<VAO>::Index) + sizeof(RMI<Material>::Index)) * BITS_PER_BYTE));

    vaoID = ResourcesManager::GetGRM().GetResourceContainer<VAO>().CompressID(
        RMI<VAO>::ID(key >> (sizeof(RMI<Material>::Index) * BITS_PER_BYTE)));

    matID = ResourcesManager::GetGRM().GetResourceContainer<Material>().CompressID(
        RMI<Material>::ID(key));

#else
    key = (Key(vao_id) << (sizeof(uint16) + sizeof(shader_id_encoding)) * BITS_PER_BYTE) | (Key(mat_id) << (sizeof(shader_id_encoding) * BITS_PER_BYTE)) | Key(sid);
#endif

    // printf("\033[1;32m* Decode : ShaderID = %d | vaoID = %d| matID = %d |-->Key = %lu\n", shaderID, vaoID, matID, key);
}

template<typename T>
void FramebufferCommandBucket<T>::Submit()
{
    Mat4f pv = scene.GetProjectionMatrix() * scene.GetCurrentCamera().GetViewMatrix();

    Key lastKey = -1;
    MaterialID lastMatID = -1;
    VaoID lastVaoID = -1;
    ShaderID lastShaderID = -1;
    FrameBufferPiriority lastFbo_pirority;
    ShaderProgram* lastShader = NULL;
    const uint32 max = m_SecondCurrent;
    const uint32 start = m_StartLocation;

    // printf("(READER THREAD) SUBMIT : Start = %d TO %d\n", start, max);

    for(uint32 i = start; i < max; i++){
        const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
        Key key = k.first;
        CmdPacket packet = BaseClass::m_Packets[k.second];

        if (key != lastKey){
            Commands::BasicDrawCommand* command = reinterpret_cast<Commands::BasicDrawCommand*>(const_cast<void*>(CommandPacket::LoadCommand(packet)));
        
            MaterialID matID;
            VaoID vaoID;
            ShaderID shaderID;
            FrameBufferPiriority fbo_pirority;
            this->DecodeKey(key, fbo_pirority, shaderID, vaoID, matID);

            if (fbo_pirority.fbo_id != lastFbo_pirority.fbo_id){
                FBO& current_fbo = ResourcesManager::GetGRM().Get<FBO>(fbo_pirority.fbo_id);
                current_fbo.Use();
                lastFbo_pirority = fbo_pirority;
            }

            if (shaderID != lastShaderID){
                lastShader = &ResourcesManager::GetGRM().Get<ShaderProgram>(shaderID);
                lastShader->Bind();
                lastShader->SetVec3("viewPos", scene.GetCurrentCamera().Position);
                lastShaderID = shaderID;
            }

            if (vaoID != lastVaoID){
                VAO& vao = ResourcesManager::GetGRM().Get<VAO>(vaoID);
                vao.Bind();
                lastVaoID = vaoID;
            }

            if(matID != lastMatID){
                Material& material = ResourcesManager::GetGRM().Get<Material>(matID);

                StateGroup& state_grp = material.GetRenderStates();
                StateHash stateHash = state_grp.GetHash();
                if (stateHash != m_LastStateHash){
                    state_grp.ApplyStates();
                    m_LastStateHash = stateHash;
                }

                if (!lastShader){
                    lastShader = &ResourcesManager::GetGRM().Get<ShaderProgram>(material.GetTechnique().GetShaderID());
                    lastShader->Bind();
                }

                lastShader->SetMat4("MVP", pv * *(command->model));
		        lastShader->SetMat4("model", *command->model);
                material.GetTechnique().UploadUnfiroms(*lastShader);
                lastMatID = matID;
            }
        }
        
        do{
            this->SubmitPacket(packet);
            packet = CommandPacket::LoadNextCommandPacket(packet);
        } while (packet != NULL);
    }

    // std::lock_guard<std::mutex> lk(mtx);
    m_IsReading = 0;
    cv.notify_one();
}

template<typename T>
void FramebufferCommandBucket<T>::Clear()
{
    if (m_StartLocation) {
        BaseClass::m_Current = 0;
    }else{
        BaseClass::m_Current = BaseClass::DEFAULT_MAX_ELEMENTS;
    }
    
    // printf("(WRITE THREAD) CLEAR : CURRENT FOR WRITE = %d\n", BaseClass::m_Current);
    BaseClass::m_CmdAllocator.SetOffset(BaseClass::m_Current * BaseClass::m_Current);
    BaseClass::m_PacketCount = 0;
}

template<typename T>
bool FramebufferCommandBucket<T>::SwapCmdBuffer()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return !m_IsReading;});

    m_SecondCurrent = BaseClass::m_Current;
    
    if (m_StartLocation){
        m_StartLocation = 0;
    }else{
        m_StartLocation = BaseClass::DEFAULT_MAX_ELEMENTS;
    }

    // printf("(WRITE THREAD) SWAP :READ WILL START FROM Start = %d | EnD =%d\n", m_StartLocation, m_SecondCurrent);

    m_IsReading = 1;
    // lk.unlock();
    return true;
}

TRE_NS_END