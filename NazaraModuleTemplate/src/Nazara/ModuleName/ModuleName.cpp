// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ModuleName.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/ModuleName/Config.hpp>
#include <Nazara/ModuleName/Debug.hpp>

bool NzModuleName::Initialize()
{
	if (s_moduleReferenceCouter++ != 0)
		return true; // D�j� initialis�

	// Initialisation des d�pendances
	if (!NzCore::Initialize())
	{
		NazaraError("Failed to initialize core module");
		return false;
	}

	// Initialisation du module

	NazaraNotice("Initialized: ModuleName module");

	return true;
}

bool NzModuleName::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
}

void NzModuleName::Uninitialize()
{
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilis�

	// Lib�ration du module

	NazaraNotice("Uninitialized: ModuleName module");

	// Lib�ration des d�pendances
	NzCore::Uninitialize();
}

unsigned int NzModuleName::s_moduleReferenceCouter = 0;
