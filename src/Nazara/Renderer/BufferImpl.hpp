// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFERIMPL_HPP
#define NAZARA_BUFFERIMPL_HPP

#include <Nazara/Renderer/Buffer.hpp>

class NzBufferImpl
{
	public:
		NzBufferImpl() = default;
		virtual ~NzBufferImpl();

		virtual void Bind() = 0;

		virtual bool Create(unsigned int size, nzBufferUsage usage = nzBufferUsage_Static) = 0;
		virtual void Destroy() = 0;

		virtual bool Fill(const void* data, unsigned int offset, unsigned int size) = 0;

		virtual void* GetBufferPtr() = 0;

		virtual bool IsHardware() const = 0;

		virtual void* Lock(nzBufferLock lock, unsigned int offset = 0, unsigned int size = 0) = 0;
		virtual bool Unlock() = 0;
};

#endif // NAZARA_BUFFERIMPL_INCLUDED
