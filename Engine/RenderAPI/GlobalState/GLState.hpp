#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <RenderAPI/Common.hpp>

TRE_NS_START

class GLState
{
public:
	template<typename T>
	FORCEINLINE static bool IsBound(const T& obj);

	template<typename T>
	FORCEINLINE static void Bind(const T& obj);

	template<typename T>
	FORCEINLINE static bool IsBound(const T* obj);

	template<typename T>
	FORCEINLINE static void Bind(const T* obj);

	template<typename T>
	FORCEINLINE static void Unbind(const T* obj);

	template<typename T>
	FORCEINLINE static void Unbind(const T& obj);
private:
	static HashMap<TargetType::target_type_t, uint32, PROBING> m_bindings;
};

template<typename T>
FORCEINLINE bool GLState::IsBound(const T& obj)
{
	return m_bindings.Get(obj.GetBindingTarget()) == obj.GetID();
}

template<typename T>
FORCEINLINE void GLState::Bind(const T& obj)
{
	if (GLState::IsBound(obj)) 
		return;

	m_bindings.GetPairPtr(obj.GetBindingTarget())->second = obj.GetID();
	obj.Bind();
}

template<typename T>
FORCEINLINE void GLState::Unbind(const T& obj)
{
	if (!GLState::IsBound(obj))
		return;

	obj.Unbind();
	m_bindings.GetPairPtr(obj.GetBindingTarget())->second = 0;
}

template<typename T>
FORCEINLINE void GLState::Bind(const T* obj)
{
	GLState::Bind(*obj);
}

template<typename T>
FORCEINLINE bool GLState::IsBound(const T* obj)
{
	return GLState::IsBound(*obj);
}

template<typename T>
FORCEINLINE void GLState::Unbind(const T* obj)
{
	GLState::Unbind(*obj);
}


TRE_NS_END