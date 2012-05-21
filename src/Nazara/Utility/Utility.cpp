// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Loaders/PCX.hpp>
#include <Nazara/Utility/Loaders/STB.hpp>
#include <Nazara/Utility/Debug.hpp>

NzUtility::NzUtility()
{
}

NzUtility::~NzUtility()
{
	if (s_initialized)
		Uninitialize();
}

bool NzUtility::Initialize()
{
	#if NAZARA_UTILITY_SAFE
	if (s_initialized)
	{
		NazaraError("Renderer already initialized");
		return true;
	}
	#endif

	// Loaders sp�cialis�s
	NzLoaders_PCX_Register(); // Loader de fichiers .PCX (1, 4, 8, 24)

	// Loaders g�n�riques (En dernier pour donner la priorit� aux loaders sp�cialis�s)
	NzLoaders_STB_Register(); // Loader g�n�rique (STB)

	s_initialized = true;

	return true;
}

void NzUtility::Uninitialize()
{
	#if NAZARA_UTILITY_SAFE
	if (!s_initialized)
	{
		NazaraError("Utility not initialized");
		return;
	}
	#endif

	NzLoaders_STB_Unregister();
	NzLoaders_PCX_Unregister();
}

bool NzUtility::IsInitialized()
{
	return s_initialized;
}

bool NzUtility::s_initialized = false;
