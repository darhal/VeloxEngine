#include "GLState.hpp"
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>

TRE_NS_START

HashMap<TargetType::target_type_t, uint32 ,PROBING> GLState::m_bindings;

TRE_NS_END