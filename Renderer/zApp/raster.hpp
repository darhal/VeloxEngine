#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include <Renderer/Common.hpp>
#include <Renderer/Window/Window.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Buffers/Buffer.hpp>
#include <Renderer/Backend/Pipeline/Pipeline.hpp>
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Misc/Color/Color.hpp>

#include "Shared.hpp"

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 viewPos;
};

void updateMVP(const TRE::Renderer::RenderBackend& backend, 
    TRE::Renderer::RingBufferHandle buffer, 
    const glm::vec3& pos = glm::vec3(-0.f, -0.f, 0.f));

void RenderFrame(TRE::Renderer::RenderBackend& backend,
    const TRE::Renderer::ShaderProgram& program,
    TRE::Renderer::GraphicsState& state,
    const TRE::Renderer::BufferHandle vertexIndexBuffer,
    // VkDescriptorSet descriptorSet,
    const TRE::Renderer::RingBufferHandle uniformBuffer,
    const TRE::Renderer::ImageViewHandle texture,
    const TRE::Renderer::SamplerHandle sampler,
    const TRE::Renderer::BufferHandle lightBuffer);


int raster(TRE::Renderer::RenderBackend& backend);
