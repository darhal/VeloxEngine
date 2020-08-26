#pragma once

#include <vector>
#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

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
		void Init(const Internal::RenderDevice& renderDevice, const std::initializer_list<ShaderStage>& shaderStages);

		uint32 GetShadersCount() const { return shadersCount; }

		const VkPipelineShaderStageCreateInfo* GetShaderStages() const { return shaderStagesCreateInfo; }
	private:
		static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
	private:
		VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[MAX_SHADER_STAGES];
		VkShaderModule shaderModules[MAX_SHADER_STAGES];
		uint32 shadersCount;
	};
}

TRE_NS_END

