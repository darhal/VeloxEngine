#pragma once

#include <vector>
#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/Pipeline/PipelineLayout/PipelineLayout.hpp>
#include <Renderer/Backend/Pipeline/VertexInput/VertexInput.hpp>

TRE_NS_START

namespace Renderer
{
	class ShaderProgram
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

		
		CONSTEXPR static uint32 MAX_DESCRIPTOR_SET = 4;

		CONSTEXPR static const char* DYNAMIC_KEYWORD_PREFIX = "DYNC_";
		CONSTEXPR static uint32 DYNAMIC_KEYWORD_SIZE = ARRAY_SIZE(DYNAMIC_KEYWORD_PREFIX);

		CONSTEXPR static const char* DEFAULT_ENTRY_POINT = "main";

		struct ShaderStage
		{
			ShaderStage(const char* path, ShaderStages stage, const char* entryPoint = DEFAULT_ENTRY_POINT) : 
				path(path), entryPoint(entryPoint), shaderStage(stage) {}

			const char* path;
			const char* entryPoint;
			ShaderStages shaderStage;
		};
	public:
		ShaderProgram() : shadersCount(0), descSetLayoutCount(0) {}

		void Create(const Internal::RenderDevice& renderDevice, const std::initializer_list<ShaderStage>& shaderStages);

		uint32 GetShadersCount() const { return shadersCount; }

		const VkPipelineShaderStageCreateInfo* GetShaderStages() const { return shaderStagesCreateInfo; }

		const PipelineLayout& GetPipelineLayout() const { return piplineLayout; };

		const DescriptorSetLayout& GetDescriptorSetLayout(uint32 i) const { return descSetLayout[i]; }

		VertexInput& GetVertexInput() { return vertexInput; }
	private:
		static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		void ReflectShaderCode(const Internal::RenderDevice& renderDevice, const void* sprivCode, size_t size, ShaderStages shaderStage);
	private:
		VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[MAX_SHADER_STAGES];
		VkShaderModule shaderModules[MAX_SHADER_STAGES];
		DescriptorSetLayout descSetLayout[PipelineLayout::MAX_LAYOUTS];
		PipelineLayout piplineLayout;
		VertexInput vertexInput;

		uint32 descSetLayoutCount;
		uint32 shadersCount;
	};
}

TRE_NS_END

