#pragma once


#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include <Renderer/Backend/Backend.hpp>

#include "cube.hpp"
#include "Shared.hpp"

struct CameraUBO;

void updateCameraUBO(const TRE::Renderer::RenderDevice& dev, TRE::Renderer::BufferHandle buffer, CameraUBO& ubo);

int rt(TRE::Renderer::RenderBackend& backend);
