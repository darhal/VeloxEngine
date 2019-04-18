#include "GLState.hpp"
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>

TRE_NS_START

HashMap<TargetType::target_type_t, uint32> GLState::m_bindings;

/*template<>
static void GLState::Bind(const ShaderProgram& obj)
{
	if (GLState::IsBound(obj)) {
		m_bindings[obj.GetBindingTarget()] = obj.GetID();
		obj.Bind();
	}
}

template<>
static void GLState::Bind(const VAO& obj)
{
	if (m_bindings[obj.GetBindingTarget()] != obj.GetID()) {
		m_bindings[obj.GetBindingTarget()] = obj.GetID();
		obj.Bind();
	}
}

template<>
static void GLState::Bind(const VBO& obj)
{
	if (m_bindings[obj.GetBindingTarget()] != obj.GetID()) {
		m_bindings[obj.GetBindingTarget()] = obj.GetID();
		obj.Bind();
	}
}

template<>
static void GLState::Bind(const Texture& obj)
{
	if (m_bindings[obj.GetBindingTarget()] != obj.GetID()) {
		m_bindings[obj.GetBindingTarget()] = obj.GetID();
		obj.Bind();
	}
}*/

TRE_NS_END