// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ModuleName.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/ModuleName/Config.hpp>

NzModuleName::NzModuleName()
{
}

NzModuleName::~NzModuleName()
{
	if (s_initialized)
		Uninitialize();
}

bool NzModuleName::Initialize()
{
	#if NAZARA_MODULENAME_SAFE
	if (s_initialized)
	{
		NazaraError("ModuleName already initialized");
		return true;
	}
	#endif

	return true;
}

void NzModuleName::Uninitialize()
{
	#if NAZARA_MODULENAME_SAFE
	if (!s_initialized)
	{
		NazaraError("ModuleName not initialized");
		return;
	}
	#endif
}

bool NzModuleName::IsInitialized()
{
	return s_initialized;
}

bool NzModuleName::s_initialized = false;
