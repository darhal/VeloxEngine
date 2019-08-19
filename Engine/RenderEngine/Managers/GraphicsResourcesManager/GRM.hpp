#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderEngine/Materials/Material.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>
#include <RenderEngine/Renderer/Backend/StateGroups/StateGroup.hpp>

TRE_NS_START

class GraphicsResourcesManager
{
public:
    // Create/Generate Function
    template<typename T, typename... Args>
    FORCEINLINE typename RMI<T>::ID Generate(Args&&... args);

    template<typename T, typename... Args>
    FORCEINLINE T* Create(typename RMI<T>::ID& id, Args&&... args);

    // Add Function
    template<typename T>
    FORCEINLINE typename RMI<T>::ID Add(T&& res);

    // Get Function
    template<typename T>
    FORCEINLINE T& Get(typename RMI<T>::ID id);

    // Remove Function
    template<typename T>
    FORCEINLINE void Remove(typename RMI<T>::ID id);

    //  Get Packed Array
    template<typename T>
    FORCEINLINE typename RMI<T>::Container& GetResourceContainer();

    FORCEINLINE StateHash AddState(const StateGroup& state);

    FORCEINLINE StateGroup& GetState(StateHash hash) const;

private:
    RMI<Texture>::Container m_Textures;
    RMI<VAO>::Container m_VAOs;
    RMI<VBO>::Container m_VBOs;
    RMI<RBO>::Container m_RBOs;
    RMI<FBO>::Container m_FBOs;
    RMI<ShaderProgram>::Container m_Shaders;
    RMI<Material>::Container m_Materials;
    HashMap<StateHash, StateGroup, PROBING> m_StatesMap;

};

using GRM = GraphicsResourcesManager;

#include "GRM.inl"

TRE_NS_END