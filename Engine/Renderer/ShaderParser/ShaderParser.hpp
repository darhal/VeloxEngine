#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

class ShaderParser
{
public:
	ShaderParser(const String& filename);

	ShaderParser(ShaderParser&& other);

	ShaderParser(const ShaderParser& other);

	String& GetCode() { return m_ShaderCode; };

	void AutoDetectUniforms(const String& code);

	void AddDefines(const String& identifier, const String& value = "");

	String Define(const Vector<String>& identifiers);
private:
	String m_ShaderCode;
	Vector<String> m_Uniforms;
	Vector<String> m_Samplers;

	CONSTEXPR static char INCLUDE_KEYWORD[] = "#include";
	CONSTEXPR static char UNIFORM_KEYWORD[] = "uniform";
	CONSTEXPR static char DEFINE_KEYWORD[] = "#define";
	const static String SHADERS_PATH;
};

TRE_NS_END