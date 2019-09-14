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

/*
The new architecture will be command is created once and submitted.
to submit to a specific target we will only pass commander ptr and the key.
*/

template<typename T>
class RenderCommandBucket : public ICommandBuffer<RenderCommandBucket<T>, T>
{
public:
    typedef T Key;
    typedef ICommandBuffer<RenderCommandBucket<T>, T> BaseClass;

    RenderCommandBucket();

	template<typename U>
	U* CreateCommand(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist = 0, usize aux_memory = 0, uint8 render_target = 0);

	template<typename U>
	U* CreateCommandOnAllRenderTargets(Key key, usize aux_memory = 0);

	template<typename U>
	U* CreateCommandOnRenderTarget(Key key, usize aux_memory = 0, uint8 render_target = 0, uint32* cmd_id = NULL);

	void AddCommandToRenderTarget(uint32 cmd_id, Key key, uint8 render_target = 0);

	void AddCommandByID(uint32 cmd_id, ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist = 0, uint8 render_target = 0);

    Key GenerateKey(ShaderID shaderID, VaoID vaoID, MaterialID matID, uint32 blend_dist = 0) const;

    bool DecodeKey(Key key, ShaderID& shaderID, VaoID& vaoID, MaterialID& matID) const;

    void Submit();

    void Clear();

    bool SwapCmdBuffer();

	void PushRenderTarget(const RenderTarget& render_target);

	void PopRenderTarget();

	RenderTarget* GetRenderTarget(uint32 index);

	const FboID& GetRenderTargetsCount() const;

	void Dump();

public:
	CONSTEXPR static uint8  READ_BUFFER = 0;
	CONSTEXPR static uint8  WRITE_BUFFER = 1;
	CONSTEXPR static uint8	NUMBER_OF_BUFFERS = 2;
	CONSTEXPR static usize  NUMBER_OF_RT = 128;//std::numeric_limits<RenderTargetID>::max();
	CONSTEXPR static uint32 CMDS_PER_RENDER_TARGETS_SIZE = (NUMBER_OF_RT - 1) * BaseClass::KEY_DEFAULT_SIZE;
	CONSTEXPR static uint32 RENDER_TARGET_SIZE = NUMBER_OF_RT * sizeof(RenderTarget);
	CONSTEXPR static uint32 CMDS_COUNT_PRE_RT_SIZE = NUMBER_OF_RT * sizeof(uint32) * NUMBER_OF_BUFFERS;
	CONSTEXPR static uint32 AUX_MEMORY = RENDER_TARGET_SIZE + CMDS_COUNT_PRE_RT_SIZE;
	CONSTEXPR static uint32 RT_KEYS_STRIDE = BaseClass::DEFAULT_MAX_ELEMENTS * NUMBER_OF_BUFFERS;

	RenderTarget* m_RenderTargetStack; // TODO: Probably make this in the manager, and retirved with an ID since it will be easier to manage and createand manage.
	uint32* m_SecondCurrent;
	uint32 m_ReadWriteKeysCountOffset[NUMBER_OF_BUFFERS];
	uint32 m_StartLocation;
    StateHash m_LastStateHash;
	FboID m_RenderTargetCount;
    std::mutex mtx;
    std::condition_variable cv;
    bool m_IsReading;
};

template<typename T>
using RenderCommandBuffer = RenderCommandBucket<T>;

#include "RenderCommandBucket.inl"

TRE_NS_END