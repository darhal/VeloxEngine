#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include <Renderer/Backend/Backend.hpp>

#include "Shared.hpp"

class Camera;

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 viewPos;
};

void updateMVP(const TRE::Renderer::RenderDevice& dev, TRE::Renderer::BufferHandle buffer, const glm::vec3& pos, Camera& cam);

void RenderFrame(TRE::Renderer::RenderDevice& dev,
    const TRE::Renderer::ShaderProgram& program,
    TRE::Renderer::GraphicsState& state,
    const TRE::Renderer::BufferHandle vertexIndexBuffer,
    // VkDescriptorSet descriptorSet,
    const TRE::Renderer::BufferHandle uniformBuffer,
    const TRE::Renderer::ImageViewHandle texture,
    const TRE::Renderer::SamplerHandle sampler,
    const TRE::Renderer::BufferHandle lightBuffer, Camera& cam);


int raster(TRE::Renderer::RenderBackend& backend);
