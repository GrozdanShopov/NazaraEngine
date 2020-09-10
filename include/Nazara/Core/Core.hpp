// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Module.hpp>
#include <Nazara/Core/TypeList.hpp>

namespace Nz
{
	class NAZARA_CORE_API Core : public Module<Core>
	{
		friend Module;

		public:
			using Dependencies = TypeList<>;

			Core();
			~Core();

		private:
			static Core* s_instance;
	};
}

#include <Nazara/Core/Core.inl>

#endif // NAZARA_CORE_HPP
