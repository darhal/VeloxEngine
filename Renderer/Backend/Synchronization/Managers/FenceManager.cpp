#include "FenceManager.hpp"
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START

void Renderer::FenceManager::Init(RenderDevice* device)
{
    this->device = device;
}

Renderer::FenceManager::~FenceManager()
{
    this->Destroy();
}

VkFence Renderer::FenceManager::RequestClearedFence()
{
    if (!fences.empty()) {
        auto ret = fences.back();
        fences.pop_back();
        return ret;
    } else {
        VkFence fence;
        VkFenceCreateInfo info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        vkCreateFence(device->GetDevice(), &info, NULL, &fence);
        return fence;
    }
}

void Renderer::FenceManager::Recycle(VkFence fence)
{
    if (fence != VK_NULL_HANDLE)
        fences.push_back(fence);
}

void Renderer::FenceManager::Destroy()
{
    for (VkFence f : fences)
        vkDestroyFence(device->GetDevice(), f, NULL);

    fences.clear();
}

TRE_NS_END