#include "ShaderParser.hpp"
#include <Core/FileSystem/File/File.hpp>

TRE_NS_START

ShaderParser::ShaderParser(const String& filename)
{
	File file(filename, File::OPEN_READ);
	m_ShaderCode = file.ReadAll();

	ssize_t include_index = m_ShaderCode.Find(INCLUDE_KEYWORD);
	const uint32 offset = include_index + sizeof(INCLUDE_KEYWORD);
	ssize_t first_quote_index = m_ShaderCode.Find("\"", offset);
	ssize_t first_quote = offset + first_quote_index + 1;
	ssize_t second_quote = m_ShaderCode.Find("\"", first_quote);
	ssize_t end_index = sizeof(INCLUDE_KEYWORD) + second_quote + first_quote_index + 2;

	String path;
	path.Copy(m_ShaderCode, first_quote, second_quote);

	printf("Path : %s\n", path.Buffer());
	printf("OFFSET:%d\n", end_index);
	File included_source(path, File::OPEN_READ);
	m_ShaderCode.Erase(include_index, end_index);
	m_ShaderCode.Insert(include_index, included_source.ReadAll());
	printf("%s\n", m_ShaderCode.Buffer());
}


TRE_NS_END