#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Backend/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	/*struct InputAssemblyState
	{
		VkPrimitiveTopology topology;
		VkBool32 primitiveRestartEnable;
	};
	
	struct RasterizationState
	{
		VkBool32                                   depthClampEnable;
		VkBool32                                   rasterizerDiscardEnable;
		VkPolygonMode                              polygonMode;
		VkCullModeFlags                            cullMode;
		VkFrontFace                                frontFace;
		VkBool32                                   depthBiasEnable;
		float                                      depthBiasConstantFactor;
		float                                      depthBiasClamp;
		float                                      depthBiasSlopeFactor;
		float                                      lineWidth;
	};

	struct MultisampleState
	{
		VkSampleCountFlagBits rasterizationSamples;
		VkBool32 sampleShadingEnable;
		float minSampleShading;
		const VkSampleMask* pSampleMask;
		VkBool32 alphaToCoverageEnable;
		VkBool32 alphaToOneEnable;
	};

		/*struct StencilOpState
	{
		VkStencilOp failOp;
		VkStencilOp passOp;
		VkStencilOp depthFailOp;
		VkCompareOp compareOp;
	};

	struct DepthStencilState
	{
		VkBool32 depthTestEnable;
		VkBool32 depthWriteEnable;
		VkCompareOp depthCompareOp;
		VkBool32 depthBoundsTestEnable;
		VkBool32 stencilTestEnable;
		StencilOpState front;
		StencilOpState back;
		float minDepthBounds;
		float maxDepthBounds;
	};
	
	struct ColorBlendState
	{
		VkBool32 logicOpEnable;
		VkLogicOp logicOp;
		uint32_t attachmentCount;
		const ColorBlendAttachmentState* pAttachments;
		float blendConstants[4];
	};
	*/

	typedef VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
	typedef VkPipelineRasterizationStateCreateInfo RasterizationState;
	typedef VkPipelineMultisampleStateCreateInfo MultisampleState;

	typedef VkStencilOpState StencilOpState;
	typedef VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	typedef VkPipelineColorBlendStateCreateInfo ColorBlendState;

	typedef VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;

	class GraphicsState
	{
	public:
		GraphicsState();

		void Reset();

		void AddViewport(const VkViewport& viewport);

		void AddScissor(const VkRect2D& rect);

		FORCEINLINE InputAssemblyState& GetInputAssemblyState() { return inputAssemblyState; };
		FORCEINLINE RasterizationState& GetRasterizationState() { return rasterizationState; }
		FORCEINLINE MultisampleState& GetMultisampleState() { return multisampleState; }
		FORCEINLINE DepthStencilState& GetDepthStencilState() { return depthStencilState; }
		FORCEINLINE ColorBlendState& GetColorBlendState() { return colorBlendState; }

		FORCEINLINE ColorBlendAttachmentState& GetColorBlendAttachmentState(uint32 i) { return colorBlendAttachmetns[i]; }
	private:
		InputAssemblyState			inputAssemblyState;
		RasterizationState			rasterizationState;
		MultisampleState			multisampleState;
		DepthStencilState			depthStencilState;
		ColorBlendState				colorBlendState;
		ColorBlendAttachmentState	colorBlendAttachmetns[MAX_ATTACHMENTS];

		VkPipelineViewportStateCreateInfo	viewportState;
		VkViewport							viewports[8];
		VkRect2D							scissors[8];

		uint32								subpassIndex;

		friend class GraphicsPipeline;
	};
}

TRE_NS_END

