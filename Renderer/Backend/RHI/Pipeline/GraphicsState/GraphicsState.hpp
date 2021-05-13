#pragma once

#include <Renderer/Backend/Common.hpp>
#include <Renderer/Backend/RHI/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
	typedef VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
	typedef VkPipelineRasterizationStateCreateInfo RasterizationState;
	typedef VkPipelineMultisampleStateCreateInfo MultisampleState;

	typedef VkStencilOpState StencilOpState;
	typedef VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	typedef VkPipelineColorBlendStateCreateInfo ColorBlendState;

	typedef VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;

	class GraphicsState : public Hashable
	{
	public:
		GraphicsState();

		void Reset();

		void AddViewport(const VkViewport& viewport);

		void AddScissor(const VkRect2D& rect);

		void SaveChanges() { this->CalculateHash(); }

		FORCEINLINE InputAssemblyState& GetInputAssemblyState() { return inputAssemblyState; };
		FORCEINLINE RasterizationState& GetRasterizationState() { return rasterizationState; }
		FORCEINLINE MultisampleState& GetMultisampleState() { return multisampleState; }
		FORCEINLINE DepthStencilState& GetDepthStencilState() { return depthStencilState; }
		FORCEINLINE ColorBlendState& GetColorBlendState() { return colorBlendState; }

		FORCEINLINE ColorBlendAttachmentState& GetColorBlendAttachmentState(uint32 i) { return colorBlendAttachmetns[i]; }

		FORCEINLINE const VkPipelineViewportStateCreateInfo& GetViewportState() const { return viewportState; }

	private:
		Hash CalculateHash();
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

		friend class Pipeline;
		friend class CommandBuffer;
	};
}

TRE_NS_END

