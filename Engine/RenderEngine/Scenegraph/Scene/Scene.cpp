#include "Scene.hpp"
#include "RenderEngine/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp"
#include "RenderEngine/Managers/RenderManager/RenderManager.hpp"

TRE_NS_START

Scene::Scene()
{
    m_Cameras.EmplaceBack(vec3(0.0f, 0.0f, 3.0f));
    m_CurrentCamera = m_Cameras.Back();
    m_Projection = mat4::perspective(m_CurrentCamera->Zoom, SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
}

Scene::Scene(const Camera& camera, const Mat4f& projection) : m_Projection(projection)
{
    m_Cameras.PushBack(camera);
    m_CurrentCamera = m_Cameras.Back();
}

Mat4f& Scene::CreateProjectionMatrix(const Vec2f& resolution, float near_plane, float far_plane)
{
    m_Projection = mat4::perspective(m_CurrentCamera->Zoom, resolution.x / resolution.y, near_plane, far_plane);
    return m_Projection; 
}

void Scene::AddMeshInstance(IPrimitiveMesh* primitive_mesh)
{
    m_Meshs.EmplaceBack(primitive_mesh);
}

void Scene::Submit()
{
    auto& cmd_buffer = RenderManager::GetRenderer().GetRenderCommandBuffer();
    
    // Submit all render objects.
    for(IPrimitiveMesh* primitive_mesh : m_Meshs){
        primitive_mesh->Submit(cmd_buffer, m_CurrentCamera->Position);
    }

    // Sort draw calls.
    cmd_buffer.Sort();
}

void Scene::Render() const
{
    RenderManager::GetRenderer().Render(*this);
}

TRE_NS_END