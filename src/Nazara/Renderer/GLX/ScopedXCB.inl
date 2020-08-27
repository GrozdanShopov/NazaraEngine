// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	template <typename T>
	ScopedXCB<T>::ScopedXCB(T* pointer) :
	m_pointer(pointer)
	{
	}

	template <typename T>
	ScopedXCB<T>::~ScopedXCB()
	{
		std::free(m_pointer);
	}

	template <typename T>
	T* ScopedXCB<T>::operator ->() const
	{
		return m_pointer;
	}

	template <typename T>
	T** ScopedXCB<T>::operator &()
	{
		return &m_pointer;
	}

	template <typename T>
	ScopedXCB<T>::operator bool() const
	{
		return m_pointer != nullptr;
	}

	template <typename T>
	T* ScopedXCB<T>::get() const
	{
		return m_pointer;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
