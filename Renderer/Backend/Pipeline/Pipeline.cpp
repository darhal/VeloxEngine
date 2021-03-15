#include "Pipeline.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

// Ray-tracing:
void Renderer::Pipeline::Create(RenderDevice& device, uint32 maxDepth, uint32 maxRayPayloadSize, uint32 maxRayHitAttribSize)
{
    renderDevice = &device;

    VkDynamicState dynamicStatesArray[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    for (uint32 i = 0; i < ARRAY_SIZE(dynamicStatesArray); i++) {
        dynamicState |= 1 << dynamicStatesArray[i];
    }

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStatesArray;

    VkRayTracingPipelineInterfaceCreateInfoKHR pipelineInterface;
    pipelineInterface.pNext = NULL;
    pipelineInterface.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR;
    pipelineInterface.maxPipelineRayPayloadSize = maxRayPayloadSize;
    pipelineInterface.maxPipelineRayHitAttributeSize = maxRayHitAttribSize;


    VkRayTracingPipelineCreateInfoKHR rayTraceInfo;
    rayTraceInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    rayTraceInfo.pNext = NULL;
    rayTraceInfo.flags = 0;

    rayTraceInfo.stageCount = (uint32)shaderProgram->GetShadersCount();
    rayTraceInfo.pStages = shaderProgram->GetShaderStages(); //shaderStagesDesc.data();
    rayTraceInfo.groupCount = shaderProgram->GetShaderGroupsCount();
    rayTraceInfo.pGroups = shaderProgram->GetShaderGroups();
    rayTraceInfo.layout = shaderProgram->GetPipelineLayout().GetApiObject();

    rayTraceInfo.maxPipelineRayRecursionDepth = maxDepth;
    rayTraceInfo.pDynamicState = NULL;//&dynamicState;
    rayTraceInfo.pLibraryInfo = NULL;
    rayTraceInfo.pLibraryInterface = NULL; // &pipelineInterface;
    rayTraceInfo.basePipelineHandle = VK_NULL_HANDLE;
    rayTraceInfo.basePipelineIndex = 0;

    VkDeferredOperationKHR deferredOperation = VK_NULL_HANDLE;
    vkCreateRayTracingPipelinesKHR(device.GetDevice(), deferredOperation, VK_NULL_HANDLE, 1, &rayTraceInfo, NULL, &pipeline);

    sbt.Init(device, *shaderProgram, *this);

    shaderProgram->DestroyShaderModules();
}

// Compute:
void Renderer::Pipeline::Create(RenderDevice& device)
{
    renderDevice = &device;

    VkComputePipelineCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.pNext = NULL;
    info.flags = 0;
    info.stage = *shaderProgram->GetShaderStages();
    info.layout = shaderProgram->GetPipelineLayout().GetApiObject();
    info.basePipelineHandle = VK_NULL_HANDLE;
    info.basePipelineIndex = -1;

    vkCreateComputePipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &info, NULL, &pipeline);

    shaderProgram->DestroyShaderModules();
}

// Graphics
void Renderer::Pipeline::Create(const RenderContext& renderContext, const VertexInput& vertexInput, const GraphicsState& state)
{
    renderDevice = renderContext.GetRenderDevice();

    VkViewport viewport;
    VkRect2D scissor;

    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = state.GetViewportState().viewportCount;
    viewportState.pViewports = state.GetViewportState().pViewports;
    viewportState.scissorCount = state.GetViewportState().scissorCount;
    viewportState.pScissors = state.GetViewportState().pScissors;

    if (state.viewportState.viewportCount == 0) {
        const Swapchain::SwapchainData& swapchainData = renderContext.GetSwapchain().GetSwapchainData();
        // state.AddViewport({ 0.f, 0.f, (float)swapchainData.swapChainExtent.width, (float)swapchainData.swapChainExtent.height, 0.f, 1.f });

        viewport = { 0.f, 0.f, (float)swapchainData.swapChainExtent.width, (float)swapchainData.swapChainExtent.height, 0.f, 1.f };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
    }

    if (state.viewportState.scissorCount == 0) {
        const Swapchain::SwapchainData& swapchainData = renderContext.GetSwapchain().GetSwapchainData();
        // state.AddScissor({ {0, 0}, swapchainData.swapChainExtent });

        scissor = { {0, 0}, swapchainData.swapChainExtent };
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
    }
    
    VkDynamicState dynamicStatesArray[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
    };

    for (uint32 i = 0; i < ARRAY_SIZE(dynamicStatesArray); i++) {
        dynamicState |= 1 << dynamicStatesArray[i];
    }

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = ARRAY_SIZE(dynamicStatesArray);
    dynamicState.pDynamicStates = dynamicStatesArray;
    
    // Add shader specilization here:
    /*{
        VectorView<uint8> specBuffer;

        VkPipelineShaderStageCreateInfo* shaderStages = shaderProgram.GetShaderStages();
        VkSpecializationInfo specInfo;
        specInfo.dataSize = specBuffer.size;
        specInfo.pData = specBuffer.data;

        specInfo.pMapEntries = 0;
        specInfo.mapEntryCount = 0;
        
        // shaderStages->pSpecializationInfo
    }*/
    
    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext                  = NULL;
    pipelineInfo.flags                  = 0;
    pipelineInfo.stageCount             = (uint32)shaderProgram->GetShadersCount();
    pipelineInfo.pStages                = shaderProgram->GetShaderStages(); //shaderStagesDesc.data();
    pipelineInfo.pVertexInputState      = &vertexInput.GetVertexInputDesc();
    pipelineInfo.pViewportState         = &viewportState;
    pipelineInfo.pInputAssemblyState    = &state.inputAssemblyState;
    pipelineInfo.pRasterizationState    = &state.rasterizationState;
    pipelineInfo.pTessellationState     = NULL;
    pipelineInfo.pMultisampleState      = &state.multisampleState;
    pipelineInfo.pDepthStencilState     = &state.depthStencilState;
    pipelineInfo.pColorBlendState       = &state.colorBlendState;
    pipelineInfo.pDynamicState          = &dynamicState;
    pipelineInfo.layout                 = shaderProgram->GetPipelineLayout().GetApiObject();
    pipelineInfo.renderPass             = renderPass->GetApiObject();
    pipelineInfo.subpass                = state.subpassIndex; //desc.subpass;
    pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE; //desc.basePipelineHandle;
    pipelineInfo.basePipelineIndex      = -1;//desc.basePipelineIndex;

    if (vkCreateGraphicsPipelines(renderContext.GetRenderDevice()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create graphics pipeline!");
    }

    shaderProgram->DestroyShaderModules();
    printf("Creating new pipline\n");
}

void Renderer::Pipeline::Create(const RenderContext& renderContext, const GraphicsState& state)
{
    this->Create(renderContext, shaderProgram->GetVertexInput(), state);
}

Renderer::Pipeline::~Pipeline()
{
    if (renderDevice) {
        if (shaderProgram) {
            shaderProgram->Destroy();
        }

        if (pipeline) {
            vkDestroyPipeline(renderDevice->GetDevice(), pipeline, NULL);
        }
    }
}

TRE_NS_END
