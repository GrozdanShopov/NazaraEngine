// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCE_HPP
#define NAZARA_RESOURCE_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzResource
{
	public:
		NzResource(bool persistent = true);
		NzResource(const NzResource& resource);
		virtual ~NzResource();

		void AddResourceReference() const;
		bool IsPersistent() const;
		void RemoveResourceReference() const;
		void SetPersistent(bool persistent = true);

	private:
		// Je fais pr�c�der le nom par 'resource' pour �viter les �ventuels conflits de noms
		mutable bool m_resourcePersistent;
		mutable unsigned int m_resourceReferenceCount;
};

#endif // NAZARA_RESOURCE_HPP
