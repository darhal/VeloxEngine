#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/DataStructure/String/String.hpp>

#include <Renderer/Backend/RenderState/RenderState.hpp>
#include <Renderer/Materials/MaterialParametres/MaterialParametres.hpp>

TRE_NS_START

class AbstractMaterial
{
public:
    typedef MaterialParametres<String> MaterialParametresContainer;

public:
    AbstractMaterial();

    AbstractMaterial(const String& name);

    TextureID AddTexture(const String& path);

    FORCEINLINE const String& GetName() const;

    FORCEINLINE const MaterialParametresContainer& GetParametres() const;

    FORCEINLINE MaterialParametresContainer& GetParametres();

    FORCEINLINE const RenderState& GetRenderStates() const;

    FORCEINLINE RenderState& GetRenderStates();
private:
    MaterialParametresContainer m_MaterialParams;
	RenderState m_StateGroup;

    String m_Name;
};

FORCEINLINE AbstractMaterial::AbstractMaterial() : 
    m_MaterialParams(),
    m_StateGroup(),
    m_Name("DEFAULT")
{
}

FORCEINLINE AbstractMaterial::AbstractMaterial(const String& name) :
    m_MaterialParams(),
    m_StateGroup(),
    m_Name(name)
{
}

FORCEINLINE const String& AbstractMaterial::GetName() const
{
    return m_Name;
}

FORCEINLINE const typename AbstractMaterial::MaterialParametresContainer& AbstractMaterial::GetParametres() const
{
    return m_MaterialParams;
}

FORCEINLINE typename AbstractMaterial::MaterialParametresContainer& AbstractMaterial::GetParametres()
{
    return m_MaterialParams;
}

FORCEINLINE const RenderState& AbstractMaterial::GetRenderStates() const
{
    return m_StateGroup;
}

FORCEINLINE RenderState& AbstractMaterial::GetRenderStates()
{
    return m_StateGroup;
}

TRE_NS_END