// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Core/Debug.hpp>

NzResourceListener::~NzResourceListener() = default;

bool NzResourceListener::OnResourceCreated(const NzResource* resource, int index)
{
	NazaraUnused(resource);
	NazaraUnused(index);

	return true;
}

bool NzResourceListener::OnResourceDestroy(const NzResource* resource, int index)
{
	NazaraUnused(resource);
	NazaraUnused(index);

	return true;
}

void NzResourceListener::OnResourceReleased(const NzResource* resource, int index)
{
	NazaraUnused(resource);
	NazaraUnused(index);
}
