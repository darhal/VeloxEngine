#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

class ShaderParser
{
public:
	ShaderParser(const String& filename);

private:
	String m_ShaderCode;
};

TRE_NS_END