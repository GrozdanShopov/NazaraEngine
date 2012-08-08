// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

bool NzNoise::Initialize()
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

	NazaraNotice("Initialized: Noise module");

	return true;
}

bool NzNoise::IsInitialized()
{
	return s_moduleReferenceCouter != 0;
}

void NzNoise::Uninitialize()
{
	if (--s_moduleReferenceCouter != 0)
		return; // Encore utilis�

	// Lib�ration du module

	// Lib�ration des d�pendances
	NzCore::Uninitialize();

	NazaraNotice("Uninitialized: Noise module");
}

unsigned int NzNoise::s_moduleReferenceCouter = 0;
