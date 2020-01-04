#include "UBO.hpp"

TRE_NS_START

UBO::UBO()
{
	// Do nothing
}

UBO::UBO(void* data, uint32 data_size) : m_VBO(BufferTarget::UNIFORM_BUFFER)
{
	m_VBO.Use();
	m_VBO.FillData(data, data_size);
	m_VBO.Unuse();
}

void UBO::Upload()
{
	for (const auto& node : m_Data) {
		m_VBO.SubFillData(node.second.data, node.second.offset, node.second.data_size);
	}
}

void UBO::Update(const String& key)
{
	const auto& node = m_Data[key];
	m_VBO.SubFillData(node.data, node.offset, node.data_size);
}

void UBO::Update(const Data& data)
{
	m_VBO.SubFillData(data.data, data.offset, data.data_size);
}

void UBO::AddData(const String& key, const Data& data)
{
	m_Data.Emplace(std::move(key), data);
}

void UBO::Init(void* data, uint32 data_size)
{
	m_VBO.Generate(BufferTarget::UNIFORM_BUFFER);
	m_VBO.Use();
	m_VBO.FillData(data, data_size);
	m_VBO.Unuse();
}

UBO::Data& UBO::GetData(const String& key)
{
	return m_Data[key];
}

const VBO& UBO::GetVBO() const
{
	return m_VBO;
}

TRE_NS_END