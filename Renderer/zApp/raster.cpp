#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Misc/Color/Color.hpp>
#include "cube.hpp"

#include "Shared.hpp"
#include "raster.hpp"

using namespace TRE::Renderer;
using namespace TRE;

/*std::vector<Vertex> vertices = {
    { TRE::vec3{-0.5f, -0.5f, 0.f},  TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f} },
    { TRE::vec3{0.5f, -0.5f, 0.f},   TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f} },
    { TRE::vec3{0.5f, 0.5f, 0.f},    TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f} },
    { TRE::vec3{-0.5f, 0.5f, 0.f},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f} },

    // Cube:
    //{ TRE::vec3{-0.5, -0.5,  0.5},   TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f}  },
    //{ TRE::vec3{0.5, -0.5,  0.5},    TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f}  },
    //{ TRE::vec3{-0.5, 0.5,  0.5},  TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f}  },
    //{ TRE::vec3{0.5, 0.5,  0.5},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f}  },
    //{ TRE::vec3{-0.5, -0.5, -0.5},   TRE::vec3{1.0f, 0.0f, 0.0f},  TRE::vec2{0.0f, 0.0f}  },
    //{ TRE::vec3{0.5, -0.5, -0.5},    TRE::vec3{0.0f, 1.0f, 0.0f},  TRE::vec2{1.0f, 0.0f}  },
    //{ TRE::vec3{-0.5, 0.5, -0.5},  TRE::vec3{0.0f, 0.0f, 1.0f},  TRE::vec2{1.0f, 1.0f}  },
    //{ TRE::vec3{0.5, 0.5, -0.5},   TRE::vec3{1.0f, 1.0f, 1.0f},  TRE::vec2{0.0f, 1.0f}  },
};*/

#define CUBE

void updateMVP(const TRE::Renderer::RenderDevice& dev, TRE::Renderer::BufferHandle buffer, const glm::vec3& pos)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    const TRE::Renderer::Swapchain::SwapchainData& swapchainData = dev.GetRenderContext()->GetSwapchain().GetSwapchainData();

    MVP mvp{};

#if defined(CUBE)
    mvp.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
#endif
    mvp.model = glm::translate(mvp.model, pos);
    mvp.model = glm::rotate(mvp.model, time * TRE::Math::ToRad(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    mvp.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj = glm::perspective<float>(TRE::Math::ToRad(45.0f), swapchainData.swapChainExtent.width / (float)swapchainData.swapChainExtent.height, 0.1f, 10.f);
    // mvp.proj    = glm::ortho(0, 1, 0, 1, 0, 1);
    mvp.proj[1][1] *= -1;

    mvp.viewPos = glm::vec3(1.0f, 1.0f, 0.7f);

    buffer->WriteToRing(sizeof(mvp), &mvp);
}

// SemaphoreHandle timeline;

void RenderFrame(TRE::Renderer::RenderDevice& dev,
                 TRE::Renderer::ShaderProgram& program,
                 TRE::Renderer::GraphicsState& state,
                 const TRE::Renderer::BufferHandle vertexIndexBuffer,
                 // VkDescriptorSet descriptorSet,
                 const TRE::Renderer::BufferHandle uniformBuffer,
                 const TRE::Renderer::ImageViewHandle texture,
                 const TRE::Renderer::SamplerHandle sampler,
                 const TRE::Renderer::BufferHandle lightBuffer)
{
    using namespace TRE::Renderer;

    CommandBufferHandle cmd = dev.RequestCommandBuffer(CommandBuffer::Type::GENERIC);

    updateMVP(dev, uniformBuffer);

    cmd->BindShaderProgram(program);
    cmd->SetGraphicsState(state);

    cmd->SetDepthTest(true, true);
    cmd->SetDepthCompareOp(VK_COMPARE_OP_LESS);
    cmd->SetCullMode(VK_CULL_MODE_NONE);
    // state.GetMultisampleState().rasterizationSamples = VkSampleCountFlagBits(backend.GetMSAASamplerCount());

    RenderPassInfo::Subpass subpass;
    cmd->BeginRenderPass(GetRenderPass(dev, subpass));

    cmd->BindVertexBuffer(*vertexIndexBuffer);
#if !defined(CUBE)
    cmd->BindIndexBuffer(*vertexIndexBuffer, sizeof(vertices[0]) * vertices.size());
#endif

    cmd->SetUniformBuffer(0, 0, *uniformBuffer);
    cmd->SetUniformBuffer(0, 2, *lightBuffer);
    cmd->SetTexture(0, 1, *texture, *sampler);

#if !defined(CUBE)
    cmd->DrawIndexed(indices.size());
#else
    cmd->Draw(36);
#endif

    cmd->EndRenderPass();
    dev.Submit(cmd);

    /*SemaphoreHandle timeline; // = dev.RequestTimelineSemaphore();
    SemaphoreHandle* semas[] = { &timeline };
    // printf("Semaphore value before wait: %d\n", timeline->GetCurrentCounterValue());
    dev.Submit(cmd, NULL, 1, semas, 1);
    dev.FlushQueues();*/
    // timeline->Wait(2);
    // printf("Semaphore value after wait: %d | Semaphore temp value: %d\n", timeline->GetCurrentCounterValue(), timeline->GetTempValue());
    // timeline->Reset();

    /*RenderPassInfo::Subpass subpass;
    cmd->BeginRenderPass(GetRenderPass(dev, subpass));
    cmd->EndRenderPass();
    dev.Submit(cmd);*/

    /*cmd->End();
    VkCommandBuffer vkcmd = cmd->GetApiObject();
    auto q = dev.GetQueue(CommandBuffer::Type::GENERIC);
    dev.SubmitCmdBuffer(q, &vkcmd, 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        dev.GetRenderContext()->GetImageAcquiredSemaphore(),
                        dev.GetRenderContext()->GetDrawCompletedSemaphore(),
                        dev.GetRenderContext()->GetFrameFence());*/
    // dev.FlushQueues();
}

FORCEINLINE uint32 NextPow2(uint32 v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

template<uint32 N>
struct BitmapTree
{
    uint32 GetLevelElementsCount(uint32 lvl)
    {
        return 1 << lvl;
    }

    uint8* GetLevel(uint32 lvl)
    {
        return &tree[(1 << lvl) - 1];
    }

    uint32 GetLeftIdx(uint32 nodeIdx)
    {
        return (nodeIdx << 1) | 1;
    }

    uint32 GetRightIdx(uint32 nodeIdx)
    {
        return (nodeIdx << 1) + 2;
    }

    uint32 GetParentIdx(uint32 nodeIdx)
    {
        if (int64(nodeIdx) - 1 < 0)
            return UINT32_MAX;
        return (nodeIdx - 1) >> 1;
    }

    uint8 GetLeftNode(uint32 nodeIdx)
    {
        return GetNode(GetLeftIdx(nodeIdx));
    }

    uint8 GetRightNode(uint32 nodeIdx)
    {
        return GetNode(GetRightIdx(nodeIdx));
    }

    uint8 GetParentNode(uint32 nodeIdx)
    {
        return GetNode(GetParentIdx(nodeIdx));
    }

    uint8 GetNode(uint32 nodeIdx)
    {
        return tree[nodeIdx];
    }

    void SetNode(uint32 nodeIdx, uint8 value)
    {
        tree[nodeIdx] = value;
    }

    void SetNodeRec(uint32 nodeIdx, uint8 value)
    {
        this->SetNode(nodeIdx, value);
        auto leftIdx = this->GetLeftIdx(nodeIdx);
        auto rightIdx = this->GetRightIdx(nodeIdx);

        if (leftIdx < N && rightIdx < N) {
            this->SetNodeRec(rightIdx, value);
            this->SetNodeRec(leftIdx, value);
        }
    }

    uint8* GetData()
    {
        return tree;
    }

    uint32 GetLevels() {
        return log2(N) + 1;
    }

    void Print()
    {
        for (uint32 i = 0; i < N; i++) {
            if (((i+1) & (i)) == 0) {
                printf( "\n");
            }

            printf("%d ", tree[i]);
        }
        printf("\n");
    }

    uint8 tree[N];
};

struct BuddyAllocator
{
    CONSTEXPR static uint32 LOG2_MAX_SIZE = 24;
    CONSTEXPR static uint32 MIN_SIZE      = 256;
    CONSTEXPR static uint32 MAX_SIZE      = 4 * MIN_SIZE;
    CONSTEXPR static uint32 NUM_BLOCKS    = MAX_SIZE / MIN_SIZE;
    CONSTEXPR static uint32 NUM_NODES     = 2 * NUM_BLOCKS - 1;

    void Init()
    {
        // TODO: allocate memory
        uint32 currentLvl = tree.GetLevels();

        for (uint32 l = 0; l < NUM_NODES; l++) {
            if (((l+1) & l) == 0) {
                currentLvl--;
            }

            tree.SetNode(l, currentLvl + 1);
        }

        tree.Print();
        printf("------------------------\n");
    }

    void Allocate(uint32 size)
    {
        uint32 currentNode = 0;
        uint32 offset = 0;
        uint32 realSize = NextPow2(size);
        uint32 level = (realSize / MIN_SIZE);
        uint32 availSize = MAX_SIZE;

        printf("Size: %d | real size: %d | lvl: %d\n", size, realSize, level);

        while (1) {
            uint32 leftIdx = tree.GetLeftIdx(currentNode);
            uint32 rightIdx = tree.GetRightIdx(currentNode);

            if (leftIdx >= NUM_NODES || rightIdx >= NUM_NODES)
                break;

            uint8 currentLevel = tree.GetNode(currentNode);
            uint8 leftNode = tree.GetLeftNode(currentNode);
            uint8 rightNode = tree.GetRightNode(currentNode);

            if (leftNode >= level) {
                currentNode = leftIdx;
                availSize >>= 1;
            }else if (rightNode >= level) {
                currentNode = rightIdx;
                availSize >>= 1;
                offset += availSize;
            }else{
                if (currentLevel < level) {
                    ASSERTF(true, "Allocator is out of memory");
                }
                break;
            }
        }

        // Mark current Node as used and then descend while updating parents
        tree.SetNode(currentNode, 0);
        uint32 dummyNode = tree.GetParentIdx(currentNode);

        while (dummyNode != UINT32_MAX) {
            uint8 leftValue = tree.GetLeftNode(dummyNode);
            uint8 rightValue = tree.GetRightNode(dummyNode);
            uint8 dummyLevel = MAX(leftValue, rightValue);

            tree.SetNode(dummyNode, dummyLevel);
            dummyNode = tree.GetParentIdx(dummyNode);
        }

        tree.Print();
        printf("Offset: %d\n", offset);
    }

    VkDeviceMemory        memory;
    BitmapTree<NUM_NODES> tree;
};

int raster(RenderBackend& backend)
{
    auto& window = *backend.GetRenderContext().GetWindow();
    Event ev;

    RenderDevice& dev = backend.GetRenderDevice();

    /*if (backend.GetMSAASamplerCount() == 1) {
        TRE_LOGI("Anti-Aliasing: Disabled");
    } else {
        TRE_LOGI("Anti-Aliasing: MSAA x%d", backend.GetMSAASamplerCount());
    }*/

    BuddyAllocator alloc;
    alloc.Init();

    /*alloc.Allocate(510);
    alloc.Allocate(256);
    alloc.Allocate(256);*/

    /*alloc.Allocate(256);
    alloc.Allocate(256);
    alloc.Allocate(257);*/

    alloc.Allocate(256);
    alloc.Allocate(257);
    alloc.Allocate(256);



    return 0;



    TRE_LOGI("Engine is up and running ...");
    Vertex vertecies[12 * 3];

#if !defined(CUBE)
    size_t vertexSize = sizeof(vertices[0]) * vertices.size();
    size_t indexSize = sizeof(indices[0]) * indices.size();
    char* data = new char[vertexSize + indexSize];
    memcpy(data, vertices.data(), vertexSize);
    memcpy(data + vertexSize, indices.data(), indexSize);

    BufferHandle vertexIndexBuffer = backend.CreateBuffer(
        { vertexSize + indexSize, BufferUsage::TRANSFER_DST | BufferUsage::VERTEX_BUFFER | BufferUsage::INDEX_BUFFER },
        data);
#else
    for (int32_t i = 0; i < 12 * 3; i++) {
        vertecies[i].pos = glm::vec3{ g_vertex_buffer_data[i * 3], g_vertex_buffer_data[i * 3 + 1], g_vertex_buffer_data[i * 3 + 2] };
        vertecies[i].tex = glm::vec2{ g_uv_buffer_data[2 * i], g_uv_buffer_data[2 * i + 1] };
        vertecies[i].color = glm::vec3{ 81.f / 255.f, 254.f / 255.f, 115.f / 255.f };
        vertecies[i].normal = glm::vec3{ g_normal_buffer_data[i * 3], g_normal_buffer_data[i * 3 + 1], g_normal_buffer_data[i * 3 + 2] };
    }

    BufferHandle vertexIndexBuffer = dev.CreateBuffer({ sizeof(vertecies), BufferUsage::VERTEX_BUFFER }, vertecies);
    // BufferHandle cpuVertexBuffer = dev.CreateBuffer({ sizeof(vertecies), BufferUsage::TRANSFER_SRC, MemoryUsage::CPU_ONLY }, vertecies);

    glm::vec4 lightInfo[] = { glm::vec4(1.f, 0.5f, 0.5f, 0.f), glm::vec4(1.f, 1.f, 1.f, 0.f) };
    BufferHandle lightBuffer = dev.CreateBuffer({ sizeof(lightInfo), BufferUsage::STORAGE_BUFFER }, lightInfo);
#endif
    const uint32 checkerboard[] = {
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,

        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
        0u, ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u,
        ~0u, 0u, ~0u, 0u, ~0u, 0u, ~0u, 0u,
    };

    // TODO: NEED WORK ON MEMORY FREEING!! (THIS IS DONE) (However we need to detect dedicated allocations from non dedicated allocs!)
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("Assets/box1.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    ImageHandle texture = dev.CreateImage(ImageCreateInfo::Texture2D(texWidth, texHeight, true), pixels);
    ImageViewHandle textureView = dev.CreateImageView(ImageViewCreateInfo::ImageView(texture, VK_IMAGE_VIEW_TYPE_2D));
    SamplerHandle sampler = dev.CreateSampler(SamplerInfo::Sampler2D(texture));
    free(pixels);

    BufferHandle uniformBuffer = dev.CreateRingBuffer(BufferInfo::UniformBuffer(sizeof(MVP)), NULL, 3);
    GraphicsState state;
    // state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_LINE;
    /*auto& depthStencilState = state.GetDepthStencilState();
    depthStencilState.depthTestEnable = true;
    depthStencilState.depthWriteEnable = true;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    state.GetRasterizationState().cullMode = VK_CULL_MODE_NONE;
    state.GetMultisampleState().rasterizationSamples = VkSampleCountFlagBits(backend.GetMSAASamplerCount());
    state.SaveChanges();*/

    ShaderProgram program(dev,
        {
            {"Shaders/vert.spv", ShaderProgram::VERTEX},
            {"Shaders/frag.spv", ShaderProgram::FRAGMENT}
        });
    program.GetVertexInput().AddBinding(
        0, sizeof(Vertex),
        VertexInput::LOCATION_0 | VertexInput::LOCATION_1 | VertexInput::LOCATION_2 | VertexInput::LOCATION_3,
        { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, tex), offsetof(Vertex, normal) }
    );
    program.Compile();

    updateMVP(dev, uniformBuffer);
    INIT_BENCHMARK;

    time_t lasttime = time(NULL);
    // TODO: shader specilization constants
    // timeline = dev.RequestTimelineSemaphore();

    //getchar();

    dev.GetStagingManager().Flush();
    dev.GetStagingManager().WaitPrevious();

    while (window.isOpen()) {
        window.getEvent(ev);

        if (ev.Type == TRE::Event::TE_RESIZE) {
            // printf("Event resize\n");
            // backend.GetRenderContext().GetSwapchain().QueueSwapchainUpdate();
            // continue;
        } else if (ev.Type == TRE::Event::TE_KEY_UP) {
            if (ev.Key.Code == TRE::Key::L) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_LINE;
                state.SaveChanges();
            } else if (ev.Key.Code == TRE::Key::F) {
                state.GetRasterizationState().polygonMode = VK_POLYGON_MODE_FILL;
                state.SaveChanges();
            }
        }

        // Using fences the performance is 1500-1800 fps
        //if (rand() % 2) {
            //printf("UPDATING VERTEX BUFFERS!\n");

            /*for (int32_t i = 0; i < 12 * 3; i++) {
                float r = ((double)rand() / (RAND_MAX)) + 1;
                vertecies[i].pos = glm::vec3{ g_vertex_buffer_data[i * 3] * r, g_vertex_buffer_data[i * 3 + 1] * r, g_vertex_buffer_data[i * 3 + 2] * r };
            }

            vertexIndexBuffer->WriteToRing(sizeof(vertecies), &vertecies);*/
        //}

        backend.BeginFrame();
        RenderFrame(dev, program, state, vertexIndexBuffer, uniformBuffer, textureView, sampler, lightBuffer);
        backend.EndFrame();
        printFPS();
    }

#if !defined(CUBE)
    delete[] data;
#endif
#if defined(OS_WINDOWS)
    getchar();
#endif
    return 0;
}
