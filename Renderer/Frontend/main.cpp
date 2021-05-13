#include <Renderer/Backend/Backend.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Frontend/zApp/Shared.hpp>

#define RASTER
#ifdef RASTER
#include <Renderer/Frontend/zApp/raster.hpp>
#else
#include <Renderer/Frontend/zApp/raytracing.hpp>
#endif

int main()
{
    using namespace TRE::Renderer;
    using namespace TRE;

    TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderBackend backend{ &window };
    // backend.SetSamplerCount(2);

#ifdef RASTER
    backend.InitInstance();
    raster(backend);
#else
    backend.InitInstance(Features::RAY_TRACING);
    rt(backend);
#endif
}
