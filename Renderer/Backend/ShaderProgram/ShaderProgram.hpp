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
			VERTEX = 0,
			FRAGMENT,
			COMPUTE,
			GEOMETRY,
			TESSELLATION_CONTROL,
			TESSELLATION_EVAL,
			END_RASTER, // this just to mark the end of rasterization shader stages

			// RT
			RGEN,
			RAHIT,
			RCHIT,
			RMISS,
			RINT,
			RCALL,

			MAX_SHADER_STAGES
		};

		CONSTEXPR static VkShaderStageFlagBits VK_SHADER_STAGES[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			VK_SHADER_STAGE_COMPUTE_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			VkShaderStageFlagBits(~0), // this just to mark the end of rasterization shader stages

			// RT:
			VK_SHADER_STAGE_RAYGEN_BIT_KHR,
			VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
			VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
			VK_SHADER_STAGE_MISS_BIT_KHR,
			VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
			VK_SHADER_STAGE_CALLABLE_BIT_KHR,
			VkShaderStageFlagBits(~0),
		};

		CONSTEXPR static const char DYNAMIC_KEYWORD_PREFIX[]  = "DYNC_";
		CONSTEXPR static const char DEFAULT_ENTRY_POINT[]	  = "main";
		CONSTEXPR static uint32 DYNAMIC_KEYWORD_SIZE		  = ARRAY_SIZE(DYNAMIC_KEYWORD_PREFIX) - 1;

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

		uint32 GetShadersCount() const { return (uint32)shaderStagesCreateInfo.size(); }

		FORCEINLINE const VkPipelineShaderStageCreateInfo* GetShaderStages() const { return shaderStagesCreateInfo.data(); }

		FORCEINLINE const PipelineLayout& GetPipelineLayout() const { return piplineLayout; };

		FORCEINLINE const VertexInput& GetVertexInput() const { return vertexInput; }

		FORCEINLINE VertexInput& GetVertexInput() { return vertexInput; }

		FORCEINLINE const VkRayTracingShaderGroupCreateInfoKHR* GetShaderGroups() const { return rtShaderGroups.data(); }

		FORCEINLINE const uint32 GetShaderGroupsCount() const { return (uint32)rtShaderGroups.size(); }

		static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
	private:
		VkPipelineShaderStageCreateInfo* GetShaderStages() { return shaderStagesCreateInfo.data(); }

		void ReflectShaderCode(const void* sprivCode, size_t size, ShaderStages shaderStage, std::unordered_set<uint32>& seenDescriptorSets,
			std::unordered_map<std::string, VkPushConstantRange>& pushConstants, uint32& oldOffset);

		VkRayTracingShaderGroupTypeKHR SetShaderGroupType(VkRayTracingShaderGroupCreateInfoKHR& group, uint32 stage, uint32 index);
	private:
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo;
		std::vector<VkShaderModule> shaderModules;
		std::vector<VkRayTracingShaderGroupCreateInfoKHR> rtShaderGroups;
		PipelineLayout piplineLayout;
		VertexInput vertexInput;

		friend class Pipeline;
	};
}

TRE_NS_END

