#include "ShaderParser.hpp"
#include <Core/FileSystem/File/File.hpp>

TRE_NS_START

ShaderParser::ShaderParser(const String& filename)
{
	File file(filename, File::OPEN_READ);
	m_ShaderCode = file.ReadAll();

	ssize_t include_index = m_ShaderCode.Find(INCLUDE_KEYWORD);

	while (include_index != -1) {
		const uint32 offset = include_index + sizeof(INCLUDE_KEYWORD);
		ssize_t first_quote_index = m_ShaderCode.Find("\"", offset);
		ssize_t first_quote = offset + first_quote_index + 1;
		ssize_t second_quote = m_ShaderCode.Find("\"", first_quote);
		ssize_t end_index = sizeof(INCLUDE_KEYWORD) + second_quote + first_quote_index + 2;

		String path;
		path.Copy(m_ShaderCode, first_quote, second_quote);

		// Parse the included file:
		ShaderParser parser(path);

		m_ShaderCode.Erase(include_index, end_index);
		m_ShaderCode.Insert(include_index, parser.GetCode());
		include_index = m_ShaderCode.Find(INCLUDE_KEYWORD);
	}
}


TRE_NS_END