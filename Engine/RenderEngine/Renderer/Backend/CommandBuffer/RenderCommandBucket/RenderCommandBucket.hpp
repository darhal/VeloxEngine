#pragma once

#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Core/Misc/Maths/Maths.hpp"
#include "Core/Misc/Defines/Common.hpp"

#include "RenderEngine/Scenegraph/Scene/Scene.hpp"
#include "RenderEngine/Renderer/Frontend/Camera/Camera.hpp"
#include "RenderEngine/Renderer/Backend/CommandPacket/CommandPacket.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/ICommandBuffer/ICommandBuffer.hpp"

TRE_NS_START

/********************************************************************************************
|-------------------------------------------------------------------------------------------|
|----------------------------------- CURRENT SYSTEM ----------------------------------------|
|  1 bit    |    23 bits     |    8 bits   |    16 bits     |   16 bits                     |
|  blending |     depth      |  shaderID   |    VaoID       |  MaterialID                   |
|-------------------------------------------------------------------------------------------|
|------------------------------------ FUTURE SYSTEM ----------------------------------------|
|  1 bit    |    16 bits     |    8 bits   |   15 bits    |    16 bits     |   16 bits      |
|  blending |     depth      |  shaderID   |   textures   |    VaoID       |  MaterialID    |
|           |                |             |   (VaoID)    |            MaterialID           |
|-------------------------------------------------------------------------------------------|
|-------------------------------------------------------------------------------------------|
********************************************************************************************/


template<typename T>
class RenderCommandBucket : public ICommandBuffer<RenderCommandBucket<T>, T, const Scene&>
{
public:
    typedef T Key;
    typedef ICommandBuffer<RenderCommandBucket<T>, T, const Scene&> BaseClass;

    RenderCommandBucket();

    Key GenerateKey(typename RMI<ShaderProgram>::ID shaderID, typename RMI<VAO>::ID vaoID, typename RMI<Material>::ID matID, uint32 blend_dist = 0) const;

    bool DecodeKey(Key key, typename RMI<ShaderProgram>::ID& shaderID, typename RMI<VAO>::ID& vaoID, typename RMI<Material>::ID& matID) const;

    void Submit(const Scene& scene);

    void Clear();

    bool SwapCmdBuffer();

public:
    uint32 m_StartLocation, m_SecondCurrent;
    StateHash m_LastStateHash;
    FboID m_MainFboID;
    std::mutex mtx;
    std::condition_variable cv;
    bool m_IsReading;
    
    CONSTEXPR static uint8  READ_CMD_BUFFER  = 0;
    CONSTEXPR static uint8  WRITE_CMD_BUFFER = 1;
};

template<typename T>
using RenderCommandBuffer = RenderCommandBucket<T>;

#include "RenderCommandBucket.inl"

TRE_NS_END