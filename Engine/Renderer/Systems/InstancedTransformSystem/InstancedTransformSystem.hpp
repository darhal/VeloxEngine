#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/System/BaseSystem.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Renderer/Components/MeshComponents/MeshInstanceComponent.hpp>
#include <Renderer/Backend/ContextOperationQueue/ContextOperationQueue.hpp>
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>

TRE_NS_START

class InstancedTransformSystem : public BaseSystem
{
public:
	InstancedTransformSystem();

	void OnUpdate(float dt) final;
private:
	void UpdateInstanceTransform(ResourcesManager& manager, ContextOperationQueue& op_queue, 
		VBO* vbo, uint32 instance_id, const Mat4f& transform);
};

TRE_NS_END