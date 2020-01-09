#pragma once

#include <utility>
#include "Core/Misc/Defines/Common.hpp"
#include "MaterialParamHelper.hpp"

TRE_NS_START

template<typename KEY_TYPE>
struct MaterialParametres
{
    template<typename T>
    using Container = MaterialHelper::Container<KEY_TYPE, T>;

    Container<mat4>      m_Mat4fs;
    Container<vec3>      m_Vec3s;
    Container<float>     m_Floats;
    Container<int32>     m_Ints;
    Container<TextureID> m_Textures;

	FORCEINLINE MaterialParametres() {};

    FORCEINLINE MaterialParametres(MaterialParametres<KEY_TYPE>&& other) :
        m_Mat4fs(std::move(other.m_Mat4fs)),
        m_Vec3s(std::move(other.m_Vec3s)),
        m_Floats(std::move(other.m_Floats)),
        m_Ints(std::move(other.m_Ints)),
        m_Textures(std::move(other.m_Textures))
    {
    }

    FORCEINLINE MaterialParametres<KEY_TYPE>& operator=(MaterialParametres<KEY_TYPE>&& other) 
    {
        m_Mat4fs = std::move(other.m_Mat4fs);
        m_Vec3s = std::move(other.m_Vec3s);
        m_Floats = std::move(other.m_Floats);
        m_Ints = std::move(other.m_Ints);
        m_Textures = std::move(other.m_Textures);
        return *this;
    }

    template<typename T>
    FORCEINLINE const Container<T>& GetContainer() const
    { 
        return MaterialHelper::GetContainer<KEY_TYPE, T>(*this);
    }

    template<typename T>
    FORCEINLINE Container<T>& GetContainer()
    { 
        return MaterialHelper::GetContainer<KEY_TYPE, T>(*this);
    }


    template<typename T, typename... Args>
    FORCEINLINE void AddParameter(const KEY_TYPE& key, Args&&... args)
    {
        MaterialHelper::GetContainer<KEY_TYPE, T>(*this).EmplaceBack(key, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    FORCEINLINE void AddParameter(KEY_TYPE&& key, Args&&... args)
    {
        MaterialHelper::GetContainer<KEY_TYPE, T>(*this).EmplaceBack(std::forward<KEY_TYPE>(key), std::forward<Args>(args)...);
    }

    FORCEINLINE void Clear()
    {
        m_Mat4fs.Clear();
        m_Vec3s.Clear();
        m_Floats.Clear();
        m_Ints.Clear();
        m_Textures.Clear();
    }
};

TRE_NS_END