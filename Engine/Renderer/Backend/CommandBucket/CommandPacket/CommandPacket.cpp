#include <algorithm>
#include "CommandPacket.hpp"
#include <Renderer/Backend/Keys/ModelKey.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <Renderer/Materials/Material.hpp>

TRE_NS_START

CommandPacket::CommandPacket(const BucketKey& key) : 
	m_CmdsAllocator(TOTAL_SIZE * MULTIPLIER, true),
	m_Key(key), m_CmdsCount(0), m_BufferMarker(0)
{
	m_Commands = (Pair<uint64, Cmd>*) m_CmdsAllocator.Allocate(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER);
}

void CommandPacket::Flush(ResourcesManager& manager, const ShaderProgram& shader)
{
	const uint32 start = m_BufferMarker * DEFAULT_MAX_ELEMENTS;
	const uint32 end = m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount;

	for (uint32 i = start; i < end; i++) {
		const uint64& internal_key = m_Commands[i].first;
		Cmd cmd = m_Commands[i].second;
		ModelRenderParams render_params = ModelRenderParams::FromKey(internal_key);

		// Bind VAO, Set Uniforms, etc...
		VAO& vao = manager.Get<VAO>(ResourcesTypes::VAO, render_params.vao_id);
		Material& material = manager.Get<Material>(ResourcesTypes::MATERIAL, render_params.material_id);
		vao.Bind();
		/*shader.SetMat4("MVP", );
		shader.SetMat4("model", );*/
		material.GetTechnique().UploadUnfiroms(shader);

		// Issue the draw call
		const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(cmd);
		const void* command = Command::LoadCommand(cmd);
		CommandFunction(command);
	}
}

void CommandPacket::SetKey(const BucketKey& key)
{
	m_Key = key;
}

void CommandPacket::SwapBuffer()
{
	m_CmdsAllocator.SetOffset(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER + DEFAULT_MAX_ELEMENTS * COMMAND_SIZE * m_BufferMarker);
	m_BufferMarker = !m_BufferMarker;	
}

void CommandPacket::SortCommands()
{
	printf("Sorting packt with key : %d\n", m_Key);

	const uint32 start = m_BufferMarker * DEFAULT_MAX_ELEMENTS;
	const uint32 end = m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount;

	std::qsort(m_Commands + start, end, sizeof(Pair<BucketKey, Cmd>), [](const void* a, const void* b) {
		const Pair<BucketKey, Cmd>& arg1 = *static_cast<const Pair<BucketKey, Cmd>*>(a);
		const Pair<BucketKey, Cmd>& arg2 = *static_cast<const Pair<BucketKey, Cmd>*>(b);

		if (arg1.first < arg2.first) return -1;
		if (arg1.first > arg2.first) return 1;
		return 0;
	});
}

TRE_NS_END