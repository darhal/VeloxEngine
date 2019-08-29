#pragma once

#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Core/Misc/Maths/Maths.hpp"
#include "Core/Misc/Defines/Common.hpp"

#include "RenderEngine/Renderer/Frontend/Camera/Camera.hpp"
#include "RenderEngine/Renderer/Backend/RenderTarget/RenderTarget.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Renderer/Backend/CommandPacket/CommandPacket.hpp"
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
class RenderCommandBucket : public ICommandBuffer<RenderCommandBucket<T>, T>
{
public:
    typedef T Key;
    typedef ICommandBuffer<RenderCommandBucket<T>, T> BaseClass;

    RenderCommandBucket();

	template<typename U>
	U* CreateCommand(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist = 0, usize aux_memory = 0);

    Key GenerateKey(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist = 0) const;

    bool DecodeKey(Key key, ShaderID& shaderID, VaoID& vaoID, MaterialID& matID) const;

    void Submit();

    void Clear();

    bool SwapCmdBuffer();

	void PushRenderTarget(const RenderTarget& render_target);

	void PopRenderTarget();

	RenderTarget* GetRenderTarget(uint32 index);
public:
	RenderTarget* m_RenderTargetStack; // TODO: Probably make this in the manager, and retirved with an ID since it will be easier to manage and createand manage.
	uint32 m_StartLocation, m_SecondCurrent; 
    StateHash m_LastStateHash;
	FboID m_RenderTargetCount;
    std::mutex mtx;
    std::condition_variable cv;
    bool m_IsReading;
    
    CONSTEXPR static uint8  READ_CMD_BUFFER		 = 0;
    CONSTEXPR static uint8  WRITE_CMD_BUFFER	 = 1;
	CONSTEXPR static uint8	NUMBER_OF_BUFFERS	 = 2;
	CONSTEXPR static uint32 AUX_MEMORY		     = std::numeric_limits<FboID>::max() * sizeof(RenderTarget);
};

template<typename T>
using RenderCommandBuffer = RenderCommandBucket<T>;

#include "RenderCommandBucket.inl"

TRE_NS_END