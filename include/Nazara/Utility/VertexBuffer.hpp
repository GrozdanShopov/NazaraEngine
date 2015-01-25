// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXBUFFER_HPP
#define NAZARA_VERTEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NzVertexBuffer;

using NzVertexBufferConstListener = NzObjectListenerWrapper<const NzVertexBuffer>;
using NzVertexBufferConstRef = NzObjectRef<NzVertexBuffer>;
using NzVertexBufferListener = NzObjectListenerWrapper<NzVertexBuffer>;
using NzVertexBufferRef = NzObjectRef<NzVertexBuffer>;

class NAZARA_API NzVertexBuffer : public NzRefCounted
{
	public:
		NzVertexBuffer() = default;
		NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer);
		NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset);
		NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzUInt32 storage = nzDataStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		NzVertexBuffer(const NzVertexBuffer& vertexBuffer);
		~NzVertexBuffer();

		bool Fill(const void* data, unsigned int startVertex, unsigned int length, bool forceDiscard = false);
		bool FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

		NzBuffer* GetBuffer() const;
		unsigned int GetEndOffset() const;
		unsigned int GetStartOffset() const;
		unsigned int GetStride() const;
		unsigned int GetVertexCount() const;
		const NzVertexDeclaration* GetVertexDeclaration() const;

		bool IsHardware() const;
		bool IsValid() const;

		void* Map(nzBufferAccess access, unsigned int startVertex = 0, unsigned int length = 0);
		void* Map(nzBufferAccess access, unsigned int startVertex = 0, unsigned int length = 0) const;
		void* MapRaw(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0);
		void* MapRaw(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

		void Reset();
		void Reset(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer);
		void Reset(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset);
		void Reset(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzUInt32 storage = nzDataStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		void Reset(const NzVertexBuffer& vertexBuffer);

		bool SetStorage(nzUInt32 storage);
		void SetVertexDeclaration(const NzVertexDeclaration* vertexDeclaration);

		void Unmap() const;

		NzVertexBuffer& operator=(const NzVertexBuffer& vertexBuffer);

		template<typename... Args> static NzVertexBufferRef New(Args&&... args);

	private:
		NzBufferRef m_buffer;
		NzVertexDeclarationConstRef m_vertexDeclaration;
		unsigned int m_endOffset;
		unsigned int m_startOffset;
		unsigned int m_vertexCount;
};

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_VERTEXBUFFER_HPP
