#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
// #include "Core/DataStructure/HashMap/HashMap.hpp"
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>

TRE_NS_START

template<typename KEY_TYPE>
struct MaterialParametres;

namespace MaterialHelper
{
    template<typename K, typename T>
    using Container = Vector<Pair<K, T>>;

    template<typename K, typename T>
    struct GetContainerImpl
    {
        static Container<K, T>& Do(MaterialParametres<K>& mat_param)
        {
            ASSERTF(true, "Attempt to call GetContainer() without valid type T.");
            return *(MaterialHelper::Container<K, T>*) NULL;
        }

        static const Container<K, T>& Do(const MaterialParametres<K>& mat_param)
        {
            ASSERTF(true, "Attempt to call GetContainer() without valid type T.");
            return *(MaterialHelper::Container<K, T>*) NULL;
        }
    };

    template<typename K, typename T>
    static Container<K, T>& GetContainer(MaterialParametres<K>& mat_param)
    {
        return GetContainerImpl<K, T>::Do(mat_param);
    }

    template<typename K, typename T>
    static const Container<K, T>& GetContainer(const MaterialParametres<K>& mat_param)
    {
        return GetContainerImpl<K, T>::Do(mat_param);
    }
};

namespace MaterialHelper
{
    template<typename K>
    struct GetContainerImpl<K, mat4>
    {
        static Container<K, mat4>& Do(MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Mat4fs;
        }

        static const Container<K, mat4>& Do(const MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Mat4fs;
        }
    };

    template<typename K>
    struct GetContainerImpl<K, vec3>
    {
        static Container<K, vec3>& Do(MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Vec3s;
        }

        static const Container<K, vec3>& Do(const MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Vec3s;
        }
    };

    template<typename K>
    struct GetContainerImpl<K, float>
    {
        static Container<K, float>& Do(MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Floats;
        }

        static const Container<K, float>& Do(const MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Floats;
        }
    };

    template<typename K>
    struct GetContainerImpl<K, int32>
    {
        static Container<K, int32>& Do(MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Ints;
        }

        static const Container<K, int32>& Do(const MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Ints;
        }
    };

    template<typename K>
    struct GetContainerImpl<K, TextureID>
    {
        static Container<K, TextureID>& Do(MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Textures;
        }

        static const Container<K, TextureID>& Do(const MaterialParametres<K>& mat_param)
        {
            return mat_param.m_Textures;
        }
    };
};

TRE_NS_END