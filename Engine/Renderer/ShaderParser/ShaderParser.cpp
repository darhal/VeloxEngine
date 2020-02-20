#include "ShaderParser.hpp"
#include <Core/FileSystem/File/File.hpp>

TRE_NS_START

ShaderParser::ShaderParser(const String& filename)
{
	File file(filename, File::OPEN_READ);
	m_ShaderCode = file.ReadAll();

	Vector<String> lines = m_ShaderCode.Split('\n');

	for (const String& l : lines)
		printf("%s\n", l.Buffer());
}



TRE_NS_END