#pragma once

#define PREPARE_VAO_ATTRIB \
	glBindVertexArray(m_ID); \
	glBindBuffer(buffer.GetTarget(), buffer); \
	glEnableVertexAttribArray(attribute); \

#define DECLARE_BIND_FUNCS \
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::FLOAT, bool>::type = true> \
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset) \
{ \
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(float), (const void*)(offset * sizeof(float))); \
} \
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::DOUBLE, bool>::type = true> \
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset) \
{ \
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(double), (const void*)(offset * sizeof(double))); \
} \
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::INT, bool>::type = true> \
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{ \
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(int32), (const void*)(offset * sizeof(int32)));\
}\
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::UINT, bool>::type = true> \
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{\
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(uint32), (const void*)(offset * sizeof(uint32)));\
}\
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::BYTE, bool>::type = true>\
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{\
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(int8), (const void*)(offset * sizeof(int8)));\
}\
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::UBYTE, bool>::type = true>\
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{\
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(uint8), (const void*)(offset * sizeof(uint8)));\
}\
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::SHORT, bool>::type = true>\
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{\
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(int16), (const void*)(offset * sizeof(int16)));\
}\
template<typename DataType::data_type_t T, typename std::enable_if<T == DataType::USHORT, bool>::type = true>\
void BindAttribute(const uint32 attribute, const VBO& buffer, uint32 count, uint32 stride, intptr offset)\
{\
	PREPARE_VAO_ATTRIB\
	glVertexAttribPointer(attribute, count, T, GL_FALSE, stride * sizeof(uint16), (const void*)(offset * sizeof(uint16)));\
}\

