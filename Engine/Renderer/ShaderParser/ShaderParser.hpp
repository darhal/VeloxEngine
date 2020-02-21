#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

class ShaderParser
{
public:
	ShaderParser(const String& filename);

	String& GetCode() { return m_ShaderCode; };
private:
	String m_ShaderCode;

	CONSTEXPR static char INCLUDE_KEYWORD[] = "#include";
};

TRE_NS_END