#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/PipelineLayout/PipelineLayout.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>


TRE_NS_START

namespace Renderer
{
	class RenderBackend;

	/*struct PushConstantKey
	{
		uint32 offset;
		uint32 size;
	};*/

	class ShaderProgram : public Hashable
	{
	public:
		enum ShaderStages
		{
			VERTEX_SHADER = 0,
			FRAGMENT_SHADER,
			COMPUTE_SHADER,
			GEOMETRY_SHADER,
			TESSELLATION_CONTROL_SHADER,
			TESSELLATION_EVAL_SHADER,
			
			MAX_SHADER_STAGES
		};

		CONSTEXPR static VkShaderStageFlagBits VK_SHADER_STAGES[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			VK_SHADER_STAGE_COMPUTE_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
		};

		CONSTEXPR static const char* DYNAMIC_KEYWORD_PREFIX = "DYNC_";
		CONSTEXPR static const char* DEFAULT_ENTRY_POINT	= "main";
		CONSTEXPR static uint32 DYNAMIC_KEYWORD_SIZE		= ARRAY_SIZE(DYNAMIC_KEYWORD_PREFIX) - 1;

		struct ShaderStage
		{
			ShaderStage(const char* path, ShaderStages stage, const char* entryPoint = DEFAULT_ENTRY_POINT) : 
				path(path), entryPoint(entryPoint), shaderStage(stage) {}

			Hash GetHash() const
			{
				Hasher h;
				h.Data(path, strlen(path));
				h.Data(entryPoint, strlen(entryPoint));
				h.u32(shaderStage);
				return h.Get();
			}

			const char* path;
			const char* entryPoint;
			ShaderStages shaderStage;
		};
	public:
		ShaderProgram(RenderBackend& renderBackend, const std::initializer_list<ShaderStage>& shaderStages);

		ShaderProgram(const ShaderProgram&) = delete;

		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void Compile();

		uint32 GetShadersCount() const { return shadersCount; }

		const VkPipelineShaderStageCreateInfo* GetShaderStages() const { return shaderStagesCreateInfo; }

		const PipelineLayout& GetPipelineLayout() const { return piplineLayout; };

		const VertexInput& GetVertexInput() const { return vertexInput; }

		VertexInput& GetVertexInput() { return vertexInput; }
	private:
		VkPipelineShaderStageCreateInfo* GetShaderStages() { return shaderStagesCreateInfo; }

		static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		void ReflectShaderCode(const void* sprivCode, size_t size, ShaderStages shaderStage, std::unordered_set<uint32>& seenDescriptorSets,
			std::unordered_map<std::string, VkPushConstantRange>& pushConstants, uint32& oldOffset);
	private:
		VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[MAX_SHADER_STAGES];
		VkShaderModule shaderModules[MAX_SHADER_STAGES];
		PipelineLayout piplineLayout;
		VertexInput vertexInput;

		uint32 shadersCount;

		friend class Pipeline;
	};
}

TRE_NS_END

