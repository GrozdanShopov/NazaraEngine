// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Debug.hpp>

bool NzCore::Initialize()
{
	if (s_moduleReferenceCouter++ != 0)
		return true; // D�j� initialis�

	// Initialisation du module
	// Le noyau de Nazara n'a pour l'instant aucun besoin d'initialisation, mais dans le futur il est tr�s probable que ce soit le cas.
	// Donc en pr�vision, tous les modules initialisent le noyau

	NazaraNotice("Initialized: Core");

	return true;
}

bool NzCore::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
}

void NzCore::Uninitialize()
{
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilis�

	// Lib�ration du module
	NazaraNotice("Uninitialized: Core");
}

unsigned int NzCore::s_moduleReferenceCouter = 0;
