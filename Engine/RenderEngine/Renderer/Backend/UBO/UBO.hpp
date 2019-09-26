#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>


TRE_NS_START

class UBO
{
public:
	struct Data {
		void* data;
		uint32 offset;
		uint32 data_size;
	};

	UBO();

	UBO(void* data, uint32 data_size);

	void Init(void* data, uint32 data_size);

	void Upload();

	void Update(const String& key);

	void Update(const Data& data);

	void AddData(const String& key, const Data& data);

	Data& GetData(const String& key);

	const VBO& GetVBO() const;

private:
	HashMap<String, Data, PROBING> m_Data;
	VBO m_VBO;
};

TRE_NS_END