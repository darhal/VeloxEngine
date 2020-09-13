#include "GraphicsPipeline.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::GraphicsPipeline::Create(const RenderContext& renderContext, const VertexInput& vertexInput, GraphicsState& state)
{
    if (state.viewportState.viewportCount == 0) {
        const Swapchain::SwapchainData& swapchainData = renderContext.GetSwapchain().GetSwapchainData();
        state.AddViewport({ 0.f, 0.f, (float)swapchainData.swapChainExtent.width, (float)swapchainData.swapChainExtent.height, 0.f, 1.f });
    }

    if (state.viewportState.scissorCount == 0) {
        const Swapchain::SwapchainData& swapchainData = renderContext.GetSwapchain().GetSwapchainData();
        state.AddScissor({ {0, 0}, swapchainData.swapChainExtent });
    }
    
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext                  = NULL;
    pipelineInfo.flags                  = 0;
    pipelineInfo.stageCount             = (uint32)shaderProgram.GetShadersCount();
    pipelineInfo.pStages                = shaderProgram.GetShaderStages(); //shaderStagesDesc.data();
    pipelineInfo.pVertexInputState      = &vertexInput.GetVertexInputDesc();
    pipelineInfo.pViewportState         = &state.viewportState;
    pipelineInfo.pInputAssemblyState    = &state.inputAssemblyState;
    pipelineInfo.pRasterizationState    = &state.rasterizationState;
    pipelineInfo.pTessellationState     = NULL;
    pipelineInfo.pMultisampleState      = &state.multisampleState;
    pipelineInfo.pDepthStencilState     = &state.depthStencilState;
    pipelineInfo.pColorBlendState       = &state.colorBlendState;
    pipelineInfo.pDynamicState          = &dynamicState;
    pipelineInfo.layout                 = shaderProgram.GetPipelineLayout().GetAPIObject();
    pipelineInfo.renderPass             = this->renderPass;
    pipelineInfo.subpass                = state.subpassIndex; //desc.subpass;
    pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE; //desc.basePipelineHandle;
    pipelineInfo.basePipelineIndex      = -1;//desc.basePipelineIndex;

    if (vkCreateGraphicsPipelines(renderContext.GetRenderDevice()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create graphics pipeline!");
    }
}

void Renderer::GraphicsPipeline::Create(const RenderContext& renderContext, GraphicsState& state)
{
    this->Create(renderContext, shaderProgram.GetVertexInput(), state);
}

TRE_NS_END
