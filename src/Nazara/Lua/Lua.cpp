// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/Lua.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Lua/Config.hpp>
#include <Nazara/Lua/Debug.hpp>

bool NzLua::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	// Initialisation des dépendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: Lua module");

	return true;
}

bool NzLua::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzLua::Uninitialize()
{
	if (s_moduleReferenceCounter != 1)
	{
		// Le module est soit encore utilisé, soit pas initialisé
		if (s_moduleReferenceCounter > 1)
			s_moduleReferenceCounter--;

		return;
	}

	// Libération du module
	s_moduleReferenceCounter = 0;

	NazaraNotice("Uninitialized: Lua module");

	// Libération des dépendances
	NzCore::Uninitialize();
}

unsigned int NzLua::s_moduleReferenceCounter = 0;
