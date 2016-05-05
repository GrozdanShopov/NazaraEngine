// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>

namespace Ndk
{
	template<typename T>
	void LuaBinding::BindComponent(const Nz::String& name)
	{
		NazaraAssert(!name.IsEmpty(), "Component name cannot be empty");

		ComponentBinding binding;
		binding.adder = &AddComponentOfType<T>;
		binding.getter = &PushComponentOfType<T>;
		binding.index = T::componentIndex;
		binding.name = name;

		if (m_componentBinding.size() <= T::componentIndex)
			m_componentBinding.resize(T::componentIndex + 1);

		m_componentBinding[T::componentIndex] = std::move(binding);
		m_componentBindingByName[name] = T::componentIndex;
	}

	template<typename T>
	int AddComponentOfType(Nz::LuaInstance& lua, EntityHandle& handle)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "ComponentType must inherit BaseComponent");

		T& component = handle->AddComponent<T>();
		lua.Push(component.CreateHandle());
		return 1;
	}

	template<typename T>
	int PushComponentOfType(Nz::LuaInstance& lua, BaseComponent& component)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "ComponentType must inherit BaseComponent");

		T& rightComponent = static_cast<T&>(component);
		lua.Push(rightComponent.CreateHandle());
		return 1;
	}
}
