#pragma once

#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

struct VariableDesc
{
    enum Frequency
    {
        FREQ_VERTEX,
        FREQ_PRIMITIVE,
        FREQ_PRIMITIVE_GROUP,
        FREQ_GEOMETRY,
        FREQ_SCENE
    };

    usize size;
    uint32 count;
    Frequency freq;

    VariableDesc() = default;

    VariableDesc(VariableDesc&& other) = default; 
    VariableDesc(const VariableDesc& other) = default; 
    VariableDesc& operator=(VariableDesc&& other) = default;
    VariableDesc& operator=(const VariableDesc& other) = default; 
};

struct VertexDataDesc : public VariableDesc
{
    enum VertAttrib
    {
        VERTEX = 0, NORMAL = 1, TEXTURE = 2
    };

    VertAttrib vert_attrib;
    uint32 indv_count[3];

    VertexDataDesc() = default;

    VertexDataDesc(VertexDataDesc&& other) = default; 
    VertexDataDesc(const VertexDataDesc& other) = default; 
    VertexDataDesc& operator=(VertexDataDesc&& other) = default;
    VertexDataDesc& operator=(const VertexDataDesc& other) = default; 
};

TRE_NS_END