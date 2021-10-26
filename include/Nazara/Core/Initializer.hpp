// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_INITIALIZER_HPP
#define NAZARA_CORE_INITIALIZER_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz
{
	template<typename... Args>
	class Initializer
	{
		public:
			Initializer(bool initialize = true);
			Initializer(const Initializer&) = delete;
			Initializer(Initializer&&) = delete; ///TODO
			~Initializer();

			bool Initialize();
			bool IsInitialized() const;
			void Uninitialize();

			explicit operator bool() const;

			Initializer& operator=(const Initializer&) = delete;
			Initializer& operator=(Initializer&&) = delete; ///TODO

		private:
			bool m_initialized;
	};
}

#include <Nazara/Core/Initializer.inl>

#endif // NAZARA_CORE_INITIALIZER_HPP
