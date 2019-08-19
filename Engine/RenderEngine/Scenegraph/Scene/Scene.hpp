#pragma once

#include "Core/Misc/Maths/Maths.hpp"
#include "Core/Misc/Defines/Common.hpp"
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

#include "RenderEngine/Renderer/Frontend/Camera/Camera.hpp"
#include "RenderEngine/Scenegraph/SceneNode/SceneNode.hpp"

TRE_NS_START

class IPrimitiveMesh;

template<typename T>
class RenderCommandBucket;

template<typename T>
class ResourcesCommandBucket;

class Scene
{
public:
    CONSTEXPR static float SCREEN_WIDTH  = 1920 / 2;
    CONSTEXPR static float SCREEN_HEIGHT = 1080 / 2;
    CONSTEXPR static float NEAR_PLANE    = 0.1f;
    CONSTEXPR static float FAR_PLANE     = 100.f;

public:
    Scene();

    Scene(const Camera& camera, const Mat4f& projection);

    Mat4f& CreateProjectionMatrix(const Vec2f& resolution, float near_plane, float far_plane);

    Camera* CreateCamera();

    FORCEINLINE const Mat4f& GetProjectionMatrix() const;

    FORCEINLINE const Camera& GetCurrentCamera() const;

    FORCEINLINE Mat4f* GetProjectionMatrix();

    FORCEINLINE Camera* GetCurrentCamera();

    void AddMeshInstance(IPrimitiveMesh* primitive_mesh);

    void Submit();

    void Render() const;

private:
    Camera* m_CurrentCamera;
    Vector<Camera> m_Cameras;
    Vector<IPrimitiveMesh*> m_Meshs;

    SceneNode m_Root;

    Mat4f m_Projection;
};

FORCEINLINE Mat4f* Scene::GetProjectionMatrix()
{
    return &m_Projection;
}

FORCEINLINE Camera* Scene::GetCurrentCamera()
{
    return m_CurrentCamera;
}

FORCEINLINE const Mat4f& Scene::GetProjectionMatrix() const
{
    return m_Projection;
}

FORCEINLINE const Camera& Scene::GetCurrentCamera() const
{
    return *m_CurrentCamera;
}

TRE_NS_END