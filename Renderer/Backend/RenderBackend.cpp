#include "RenderBackend.hpp"
#include <Renderer/Backend/RenderInstance/RenderInstance.hpp>
#include <Renderer/Backend/Common/Globals.hpp>
#include <Renderer/Backend/RenderContext/RenderContext.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>
#include <Renderer/Backend/Swapchain/Swapchain.hpp>
#include <Renderer/Backend/Images/Image.hpp>

TRE_NS_START

Renderer::RenderBackend::RenderBackend(TRE::Window* wnd) :
    window(wnd),
    renderContext(*this),
    renderDevice(&renderContext),
    msaaSamplerCount(1)
{
    renderDevice.internal.renderContext = &renderContext.internal;
    renderContext.internal.renderDevice = &renderDevice.internal;
}

void Renderer::RenderBackend::InitInstance(uint32 usage)
{
    enabledFeatures = usage;
    StaticVector<const char*> deviceExt;

    if (usage & RAY_TRACING) {
        for (auto ext : DEV_EXTENSIONS[GetSetBit(RAY_TRACING)]) {
            deviceExt.PushBack(ext);
        }
    }

    renderInstance.CreateRenderInstance();

    renderContext.CreateRenderContext(window, renderInstance.internal);
    renderDevice.CreateRenderDevice(renderInstance, deviceExt.begin(), deviceExt.Size());
    renderContext.InitRenderContext(renderInstance.internal, renderDevice.internal);

    renderDevice.Init(usage);

    const auto vendor = [](uint32 id) -> std::string
    {
        switch (id) {
        case 0x1002:
            return "AMD";
            break;
        case 0x10DE:
            return "NIVIDIA";
            break;
        case 0x8086:
            return "Intel";
            break;
        case 0x13B5:
            return "ARM";
            break;
        case 0x5143:
            return "Qualcomm";
            break;
        case 0x1010:
            return "Imagination Technology";
            break;
        default:
            return "Unknown";
        }
    };

    TRE_LOGI("GPU............: %s", renderDevice.internal.gpuProperties.deviceName);
    TRE_LOGI("Vendor.........: %s", vendor(renderDevice.internal.gpuProperties.vendorID).c_str());
    TRE_LOGI("Driver.........: %d.%d", VK_VERSION_MAJOR(renderDevice.internal.gpuProperties.driverVersion), VK_VERSION_MINOR(renderDevice.internal.gpuProperties.driverVersion));
    TRE_LOGI("Device ID......: 0x%x", renderDevice.internal.gpuProperties.deviceID);
}


Renderer::RenderBackend::~RenderBackend()
{
    vkDeviceWaitIdle(renderDevice.internal.device);

    renderContext.DestroyRenderContext(renderInstance.internal, renderDevice.internal, renderContext.internal);
    renderDevice.Shutdown();
    renderInstance.DestroyRenderInstance();
}


void Renderer::RenderBackend::BeginFrame()
{
    renderContext.BeginFrame(renderDevice);
    renderDevice.BeginFrame();
}

void Renderer::RenderBackend::EndFrame()
{
    renderDevice.EndFrame();
    renderContext.EndFrame(renderDevice);
}

TRE_NS_END
