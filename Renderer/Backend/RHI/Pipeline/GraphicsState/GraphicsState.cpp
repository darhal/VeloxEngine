#include "GraphicsState.hpp"

TRE_NS_START


CONSTEXPR static Renderer::InputAssemblyState defaultInputAssemblyState = {
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, NULL, 0,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    false
};

CONSTEXPR static Renderer::RasterizationState defaultRasterizationState = {
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, NULL, 0,
    false,
    false,
    VK_POLYGON_MODE_FILL,
    // VK_CULL_MODE_FRONT_BIT,
    VK_CULL_MODE_NONE,
    VK_FRONT_FACE_CLOCKWISE,
    false,
    0.0f,
    0.0f,
    0.0f,
    1.0f
};

CONSTEXPR static Renderer::MultisampleState defaultMultisampleState = {
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, NULL, 0,
    VK_SAMPLE_COUNT_1_BIT,
    false,
    1.0f,
    NULL,
    false,
    false
};

CONSTEXPR static Renderer::DepthStencilState defaultDepthStencilState = {
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, NULL, 0,
    false,
    true,
    VK_COMPARE_OP_LESS_OR_EQUAL,
    false,
    false,
    {
        VK_STENCIL_OP_REPLACE,
        VK_STENCIL_OP_REPLACE,
        VK_STENCIL_OP_REPLACE,
        VK_COMPARE_OP_NEVER,
        0,
        0, 
        0
    },
    {
        VK_STENCIL_OP_REPLACE,
        VK_STENCIL_OP_REPLACE,
        VK_STENCIL_OP_REPLACE,
        VK_COMPARE_OP_NEVER,
        0,
        0,
        0
    },
    0.f,
    1.f
};

CONSTEXPR static Renderer::ColorBlendState defaultColorBlendState = {
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, NULL, 0,
    false,
    VK_LOGIC_OP_SET,
    0,
    NULL,
    {1.0f, 1.0f, 1.0f, 1.0f}
};

CONSTEXPR static Renderer::ColorBlendAttachmentState defaultColorBlendAttachmentState = {
    false,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
};

Renderer::GraphicsState::GraphicsState()
{
    Reset();
}

void Renderer::GraphicsState::Reset()
{
    memcpy(&inputAssemblyState, &defaultInputAssemblyState, sizeof(InputAssemblyState));
    memcpy(&rasterizationState, &defaultRasterizationState,sizeof(RasterizationState));;
    memcpy(&multisampleState, &defaultMultisampleState, sizeof(MultisampleState));
    memcpy(&depthStencilState, &defaultDepthStencilState, sizeof(DepthStencilState));
    memcpy(&colorBlendState, &defaultColorBlendState, sizeof(ColorBlendState));

    for (uint32 i = 0; i < 8; i++)
        memcpy(&colorBlendAttachmetns[i], &defaultColorBlendAttachmentState, sizeof(ColorBlendAttachmentState));

    colorBlendState.attachmentCount = 1; // TODO: This have to have its own functionality
    colorBlendState.pAttachments = &colorBlendAttachmetns[0];

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = NULL;
    viewportState.flags = 0;
    viewportState.pViewports = viewports;
    viewportState.viewportCount = 0;
    viewportState.pScissors = scissors;
    viewportState.scissorCount = 0;

    subpassIndex = 0;
}

void Renderer::GraphicsState::AddViewport(const VkViewport& viewport)
{
    // TODO: perform a size check

    memcpy(&viewports[viewportState.viewportCount], &viewport, sizeof(VkViewport));
    viewportState.viewportCount++;
}

void Renderer::GraphicsState::AddScissor(const VkRect2D& rect)
{
    // TODO: perform a size check

    memcpy(&scissors[viewportState.scissorCount], &rect, sizeof(VkRect2D));
    viewportState.scissorCount++;
}

Renderer::Hash Renderer::GraphicsState::CalculateHash()
{
    Hasher h;

    h.u32(inputAssemblyState.topology);
    h.u32(inputAssemblyState.primitiveRestartEnable);

    h.u32(rasterizationState.depthClampEnable);
    h.u32(rasterizationState.rasterizerDiscardEnable);
    h.u32(rasterizationState.polygonMode);
    h.u32(rasterizationState.cullMode);
    h.u32(rasterizationState.frontFace);
    h.u32(rasterizationState.depthBiasEnable);
    h.f32(rasterizationState.depthBiasConstantFactor);
    h.f32(rasterizationState.depthBiasClamp);
    h.f32(rasterizationState.depthBiasSlopeFactor);
    h.f32(rasterizationState.lineWidth);

    h.u32(multisampleState.rasterizationSamples);
    h.u32(multisampleState.sampleShadingEnable);
    h.f32(multisampleState.minSampleShading);
    h.u64((uint64)multisampleState.pSampleMask);
    h.u32(multisampleState.alphaToCoverageEnable);
    h.u32(multisampleState.alphaToOneEnable);
    
    h.u32(depthStencilState.depthTestEnable);
    h.u32(depthStencilState.depthWriteEnable);
    h.u32(depthStencilState.depthCompareOp);
    h.u32(depthStencilState.depthBoundsTestEnable);
    h.u32(depthStencilState.stencilTestEnable);
    h.Data(reinterpret_cast<const uint32*>(&depthStencilState.front), sizeof(VkStencilOpState) / 4);
    h.Data(reinterpret_cast<const uint32*>(&depthStencilState.back), sizeof(VkStencilOpState) / 4);
    h.f32(depthStencilState.minDepthBounds);
    h.f32(depthStencilState.maxDepthBounds);

    h.u32(colorBlendState.logicOpEnable);
    h.u32(colorBlendState.logicOp);
    h.u32(colorBlendState.attachmentCount);

    for (uint32 i = 0; i < colorBlendState.attachmentCount; i++) {
        const auto& attch = colorBlendState.pAttachments[i];
        h.u32(attch.blendEnable);
        h.u32(attch.srcColorBlendFactor);
        h.u32(attch.dstColorBlendFactor);
        h.u32(attch.colorBlendOp);
        h.u32(attch.srcAlphaBlendFactor);
        h.u32(attch.dstAlphaBlendFactor);
        h.u32(attch.alphaBlendOp);
        h.u32(attch.colorWriteMask);
    }

    for (uint32 i = 0; i < 4; i++)
        h.f32(colorBlendState.blendConstants[i]);

    hash = h.Get();
    return h.Get();
}

TRE_NS_END
