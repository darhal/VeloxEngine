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

//#include <Engine/Core/Misc/Maths/Maths.hpp>
//#include <Engine/Core/Misc/Utils/Logging.hpp>
#include "cube.hpp"

#include "Shared.hpp"

struct CameraUBO;

void updateCameraUBO(const TRE::Renderer::RenderDevice& dev, TRE::Renderer::RingBufferHandle buffer, CameraUBO& ubo);

int rt(TRE::Renderer::RenderBackend& backend);
