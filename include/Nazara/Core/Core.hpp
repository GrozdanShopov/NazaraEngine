// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>

class NAZARA_API NzCore
{
	public:
		NzCore() = delete;
		~NzCore() = delete;

		static bool Initialize();

		static bool IsInitialized();

		static void Uninitialize();

	private:
		static unsigned int s_moduleReferenceCouter;
};

#endif // NAZARA_CORE_HPP
