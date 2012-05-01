// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Resource.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>

NzResource::NzResource(bool persistent) :
m_resourcePersistent(persistent),
m_resourceReferenceCount(0)
{
}

NzResource::~NzResource() = default;

void NzResource::AddResourceReference() const
{
	m_resourceReferenceCount++;
}

bool NzResource::IsPersistent() const
{
	return m_resourcePersistent;
}

void NzResource::RemoveResourceReference() const
{
	#if NAZARA_UTILITY_SAFE
	if (m_resourceReferenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return;
	}
	#endif

	if (--m_resourceReferenceCount == 0 && !m_resourcePersistent)
		delete this;
}

void NzResource::SetPersistent(bool persistent)
{
	m_resourcePersistent = persistent;

	if (!persistent && m_resourceReferenceCount == 0)
		delete this;
}
