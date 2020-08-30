#include "GraphicsPipeline.hpp"
#include <Renderer/Backend/Common/Utils.hpp>

TRE_NS_START

VkShaderModule Renderer::Internal::CreateShaderModule(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        ASSERTF(true, "failed to create shader module!");
    }

    return shaderModule;
}


void Renderer::GraphicsPipeline::Create(const Internal::RenderContext& renderContext, const VertexInput& vertexInput, GraphicsState& state)
{
    if (state.viewportState.viewportCount == 0) {
        state.AddViewport({ 0.f, 0.f, (float)renderContext.swapChainData.swapChainExtent.width, (float)renderContext.swapChainData.swapChainExtent.height, 0.f, 1.f });
    }

    if (state.viewportState.scissorCount == 0) {
        state.AddScissor({ {0, 0}, renderContext.swapChainData.swapChainExtent });
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

    if (vkCreateGraphicsPipelines(renderContext.renderDevice->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline) != VK_SUCCESS) {
        ASSERTF(true, "Failed to create graphics pipeline!");
    }
}

void Renderer::GraphicsPipeline::Create(const Internal::RenderContext& renderContext, GraphicsState& state)
{
    this->Create(renderContext, shaderProgram.GetVertexInput(), state);
}

void Renderer::Internal::CreateGraphicsPipeline(const RenderDevice& renderDevice, GraphicsPipeline2& pipline, const GraphicsPiplineDesc& desc)
{
    TRE::Vector<VkVertexInputBindingDescription> vertexInputBindings(desc.vertexInputDesc.Size());
    TRE::Vector<VkVertexInputAttributeDescription> vertexInputAttribs(desc.vertexInputDesc.Size()); // it takes more than this size

    for (const VertexInputDesc& VIDesc : desc.vertexInputDesc) {
        vertexInputBindings.EmplaceBack(VkVertexInputBindingDescription{VIDesc.binding, VIDesc.stride, VIDesc.inputRate});

        for (const VertextAttribDesc& attrib : VIDesc.attribDesc) {
            vertexInputAttribs.EmplaceBack(VkVertexInputAttributeDescription{attrib.location, VIDesc.binding, attrib.format, attrib.offset});
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = (uint32)vertexInputBindings.Size();
    vertexInputInfo.pVertexBindingDescriptions      = vertexInputBindings.Data();
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32)vertexInputAttribs.Size();
    vertexInputInfo.pVertexAttributeDescriptions    = vertexInputAttribs.Data();


    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology                  = desc.inputAssemblyDesc.topology;
    inputAssembly.primitiveRestartEnable    = desc.inputAssemblyDesc.primitiveRestartEnable;


    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount     = 1;
    viewportState.pViewports        = &desc.viewport;
    viewportState.scissorCount      = 1;
    viewportState.pScissors         = &desc.scissor;


    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode              = desc.rasterStateDesc.polygonMode;
    rasterizer.cullMode                 = desc.rasterStateDesc.cullMode;
    rasterizer.frontFace                = desc.rasterStateDesc.frontFace;
    rasterizer.lineWidth                = desc.rasterStateDesc.lineWidth;
    rasterizer.depthClampEnable         = desc.rasterStateDesc.depthClampEnable;
    rasterizer.rasterizerDiscardEnable  = desc.rasterStateDesc.rasterizerDiscardEnable;
    rasterizer.depthBiasEnable          = desc.rasterStateDesc.depthBiasEnable;
    rasterizer.depthBiasConstantFactor  = desc.rasterStateDesc.depthBiasConstantFactor;     // Optional
    rasterizer.depthBiasClamp           = desc.rasterStateDesc.depthBiasClamp;              // Optional
    rasterizer.depthBiasSlopeFactor     = desc.rasterStateDesc.depthBiasSlopeFactor;        // Optional


    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable      = VK_FALSE; // Optional


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask         = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable            = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor    = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor    = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp           = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor    = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor    = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp           = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional


    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount  = 2;
    dynamicState.pDynamicStates     = dynamicStates;


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount           = (uint32)desc.piplineLayoutDesc.descriptorSetLayout.Size(); // Optional
    pipelineLayoutInfo.pSetLayouts              = desc.piplineLayoutDesc.descriptorSetLayout.Data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount   = (uint32)desc.piplineLayoutDesc.pushConstantRanges.Size(); // Optional
    pipelineLayoutInfo.pPushConstantRanges      = desc.piplineLayoutDesc.pushConstantRanges.Data(); // Optional

    if (vkCreatePipelineLayout(renderDevice.device, &pipelineLayoutInfo, NULL, &pipline.pipelineLayout) != VK_SUCCESS) {
        ASSERTF(true, "failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount             = (uint32)desc.shaderStagesDesc.Size();
    pipelineInfo.pStages                = desc.shaderStagesDesc.Data();
    pipelineInfo.pVertexInputState      = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState    = &inputAssembly;
    pipelineInfo.pViewportState         = &viewportState;
    pipelineInfo.pRasterizationState    = &rasterizer;
    pipelineInfo.pMultisampleState      = &multisampling;
    pipelineInfo.pDepthStencilState     = NULL; // Optional
    pipelineInfo.pColorBlendState       = &colorBlending;
    pipelineInfo.pDynamicState          = &dynamicState; // Optional
    pipelineInfo.layout                 = pipline.pipelineLayout;
    pipelineInfo.renderPass             = pipline.renderPass;
    pipelineInfo.subpass                = desc.subpass;
    pipelineInfo.basePipelineHandle     = desc.basePipelineHandle; // Optional
    pipelineInfo.basePipelineIndex      = desc.basePipelineIndex;  // Optional

    if (vkCreateGraphicsPipelines(renderDevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipline.pipeline) != VK_SUCCESS) {
        ASSERTF(true, "failed to create graphics pipeline!");
    }
}

void Renderer::Internal::CreateRenderPass(const RenderDevice& renderDevice, VkRenderPass* renderPass, const RenderPassDesc& desc)
{
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount  = (uint32)desc.attachments.Size();
    renderPassInfo.pAttachments     = desc.attachments.Data();
    renderPassInfo.subpassCount     = (uint32) desc.subpassesDesc.Size();
    renderPassInfo.pSubpasses       = desc.subpassesDesc.Data();
    renderPassInfo.dependencyCount  = (uint32)desc.subpassDependency.Size();
    renderPassInfo.pDependencies    = desc.subpassDependency.Data();

    if (vkCreateRenderPass(renderDevice.device, &renderPassInfo, nullptr, renderPass) != VK_SUCCESS) {
        ASSERTF(true, "failed to create render pass!");
    }
}

TRE_NS_END
