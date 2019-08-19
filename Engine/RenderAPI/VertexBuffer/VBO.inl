
FORCEINLINE uint32 VBO::Generate(BufferTarget::buffer_target_t target)
{
	m_target = target;
	Call_GL(
		glGenBuffers(1, &m_ID)
	);
	return m_ID;
}

FORCEINLINE VBO::VBO(BufferTarget::buffer_target_t target) : m_ID(0), m_target(target)
{
	Call_GL(
		glGenBuffers(1, &m_ID)
	);
}

FORCEINLINE void VBO::FillData(const void* data, ssize_t size, BufferUsage::buffer_usage_t usage)
{
	//ASSERTF(m_target == BufferTarget::UNKNOWN, "Attempt to fill data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	this->Bind(); //glBindBuffer(m_target, m_ID);
	Call_GL(
		glBufferData(m_target, size, data, usage)
	);
}

FORCEINLINE void VBO::SubFillData(const void* data, ssize_t offset, ssize_t length)
{
	//ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to fill sub data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	this->Bind(); //glBindBuffer(m_target, m_ID);
	Call_GL(
		glBufferSubData(m_target, offset, length, data)
	);
}

FORCEINLINE void VBO::GetSubData(void* data, ssize_t offset, ssize_t length)
{
	//ASSERTF(m_target != BufferTarget::UNKNOWN, "Attempt to get sub data of a vertex buffer object without setting the target (VBO ID = %d_n)", m_ID);
	this->Bind(); //glBindBuffer(m_target, m_ID);
	Call_GL(
		glGetBufferSubData(m_target, offset, length, data)
	);
}

FORCEINLINE void VBO::Bind() const
{
	ASSERTF(m_ID == 0, "Attempt to bind a vertex buffer without generating it (VBO ID = %d_n)", m_ID);
	ASSERTF(m_target == BufferTarget::UNKNOWN, "Attempt to bind a vertex buffer object without setting the target (VBO ID = %d_n)", m_ID);
	Call_GL(
		glBindBuffer(m_target, m_ID)
	);
}

FORCEINLINE void VBO::Use() const
{
	GLState::Bind(this);
}

FORCEINLINE void VBO::Unbind() const
{
	ASSERTF(m_ID == 0, "Attempt to bind a vertex buffer without generating it (VBO ID = %d_n)", m_ID);
	ASSERTF(m_target == BufferTarget::UNKNOWN, "Attempt to unbind a vertex buffer object without setting the target (VBO ID = %d_n)", m_ID);
	Call_GL(
		glBindBuffer(m_target, 0)
	);
}

FORCEINLINE void VBO::Unuse() const
{
	GLState::Unbind(this);
}

FORCEINLINE VBO::~VBO()
{
	if (m_ID) {
		this->Clean();
		m_ID = 0;
	}
}

FORCEINLINE void VBO::Clean()
{
	if (m_ID) {
		Call_GL(
			glDeleteBuffers(1, &m_ID)
		);
		m_ID = 0;
	}
}

FORCEINLINE const uint32 VBO::GetID() const
{ 
	return m_ID; 
}

FORCEINLINE VBO::operator uint32() const 
{ 
	return m_ID; 
}

FORCEINLINE const int32 VBO::GetTarget() const
{
	return (int32)m_target;
}

template<typename T, ssize_t N>
FORCEINLINE void VBO::FillData(T(&data)[N], BufferUsage::buffer_usage_t usage)
{
	ASSERTF(m_target == BufferTarget::UNKNOWN, "Attempt to fill data of a vertex buffer object without setting the target (VBO ID = %d)", m_ID);
	Call_GL(
		glBindBuffer(m_target, m_ID)
	);
	Call_GL(
		glBufferData(m_target, sizeof(data), data, usage)
	);
}

FORCEINLINE VBO::VBO(VBO&& other) : m_ID(other.m_ID), m_target(other.m_target)
{
	other.m_ID = 0;
	other.m_target = BufferTarget::UNKNOWN;
}

FORCEINLINE VBO& VBO::operator=(VBO&& other)
{
	m_ID = other.m_ID;
	m_target = other.m_target;
	other.m_ID = 0;
	other.m_target = BufferTarget::UNKNOWN;
	return *this;
}