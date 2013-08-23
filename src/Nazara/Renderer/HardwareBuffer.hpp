// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREBUFFER_HPP
#define NAZARA_HARDWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>

class NzHardwareBuffer : public NzAbstractBuffer
{
	public:
		NzHardwareBuffer(NzBuffer* parent, nzBufferType type);
		~NzHardwareBuffer();

		bool Create(unsigned int size, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard);

		bool IsHardware() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0);
		bool Unmap();

		// Fonctions OpenGL
		void Bind() const;
		unsigned int GetOpenGLID() const;

	private:
		GLuint m_buffer;
		nzBufferType m_type;
		NzBuffer* m_parent;
};

#endif // NAZARA_HARDWAREBUFFER_HPP
