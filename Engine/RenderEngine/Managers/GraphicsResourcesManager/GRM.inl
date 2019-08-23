template<typename T, typename... Args>
FORCEINLINE typename RMI<T>::ID GraphicsResourcesManager::Generate(Args&&... args)
{
    typename RMI<T>::Container& container = GraphicsResourcesManager::GetResourceContainer<T>();
    return container.Emplace(std::forward<Args>(args)...);
}

// Create Function
template<typename T, typename... Args>
FORCEINLINE T* GraphicsResourcesManager::Create(typename RMI<T>::ID& id, Args&&... args)
{
    typename RMI<T>::Container& container = GraphicsResourcesManager::GetResourceContainer<T>();
    id = container.Emplace(std::forward<Args>(args)...);
    return &container.Lookup(id).second;
}

template<typename T>
FORCEINLINE typename RMI<T>::ID GraphicsResourcesManager::Add(T&& res)
{
    typename RMI<T>::Container& container = GraphicsResourcesManager::GetResourceContainer<T>();
    return container.Add(std::forward<T>(res));
}

template<typename T>
FORCEINLINE void GraphicsResourcesManager::Remove(typename RMI<T>::ID id)
{
    typename RMI<T>::Container& container = GraphicsResourcesManager::GetResourceContainer<T>();
    container.Remove(id);
}

template<typename T>
FORCEINLINE T& GraphicsResourcesManager::Get(typename RMI<T>::ID id)
{
    typename RMI<T>::Container& container = GraphicsResourcesManager::GetResourceContainer<T>();
    return container.Lookup(id).second;
}

template<typename T>
FORCEINLINE typename RMI<T>::Container& GraphicsResourcesManager::GetResourceContainer()
{
    ASSERTF(true, "Error in attempt to call GetResourceContainer() T must be specilized!");
    return RMI<T>::Container();
}

template<>
FORCEINLINE typename RMI<Texture>::Container& GraphicsResourcesManager::GetResourceContainer<Texture>()
{
    return m_Textures;
}

template<>
FORCEINLINE typename RMI<VAO>::Container& GraphicsResourcesManager::GetResourceContainer<VAO>()
{
    return m_VAOs;
}

template<>
FORCEINLINE typename RMI<VBO>::Container& GraphicsResourcesManager::GetResourceContainer<VBO>()
{
    return m_VBOs;
}

template<>
FORCEINLINE typename RMI<RBO>::Container& GraphicsResourcesManager::GetResourceContainer<RBO>()
{
    return m_RBOs;
}

template<>
FORCEINLINE typename RMI<FBO>::Container& GraphicsResourcesManager::GetResourceContainer<FBO>()
{
    return m_FBOs;
}

template<>
FORCEINLINE typename RMI<ShaderProgram>::Container& GraphicsResourcesManager::GetResourceContainer<ShaderProgram>()
{
    return m_Shaders;
}

template<>
FORCEINLINE typename RMI<Material>::Container& GraphicsResourcesManager::GetResourceContainer<Material>()
{
    return m_Materials;
}

template<>
FORCEINLINE typename RMI<RenderTarget>::Container& GraphicsResourcesManager::GetResourceContainer<RenderTarget>()
{
	return m_RenderTargets;
}