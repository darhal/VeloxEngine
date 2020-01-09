#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderAPI/General/GLContext.hpp"

TRE_NS_START

struct PrimitiveGeometry
{
public:
    PrimitiveGeometry(DataType::data_type_t data_type, int32 count, intptr offset = 0, 
        Primitive::primitive_t primitive = Primitive::TRIANGLES) : 
        m_Primitive(primitive), m_Count(count), m_Offset(offset), m_DataType(data_type)
    {}

    PrimitiveGeometry(int32 start, int32 end, 
        Primitive::primitive_t primitive = Primitive::TRIANGLES) : 
        m_Primitive(primitive), m_Start(start), m_End(end)
    {}

    const Primitive::primitive_t& GetPrimitive() const { return m_Primitive; }

    Primitive::primitive_t m_Primitive;

    union {
        struct {
            int32 m_Count; 
            intptr m_Offset;
            DataType::data_type_t m_DataType; 
        };

        struct {
            int32 m_Start;
            int32 m_End;
        };
    };
};

TRE_NS_END