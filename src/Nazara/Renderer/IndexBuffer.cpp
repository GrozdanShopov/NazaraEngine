// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/IndexBuffer.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzIndexBuffer::NzIndexBuffer(NzBuffer* buffer, unsigned int startIndex, unsigned int indexCount) :
m_buffer(buffer),
m_ownsBuffer(false),
m_indexCount(indexCount),
m_startIndex(startIndex)
{
	if (m_buffer)
		m_buffer->AddResourceReference();
}

NzIndexBuffer::NzIndexBuffer(unsigned int length, nzUInt8 indexSize, nzBufferUsage usage) :
m_ownsBuffer(true),
m_indexCount(length),
m_startIndex(0)
{
	m_buffer = new NzBuffer(nzBufferType_Index, length, indexSize, usage);
	m_buffer->AddResourceReference();
	m_buffer->SetPersistent(false);
}

NzIndexBuffer::NzIndexBuffer(const NzIndexBuffer& indexBuffer) :
m_buffer(indexBuffer.m_buffer),
m_indexCount(indexBuffer.m_indexCount),
m_startIndex(indexBuffer.m_startIndex)
{
	if (m_buffer)
	{
		if (m_ownsBuffer)
		{
			m_buffer = new NzBuffer(nzBufferType_Index, indexBuffer.m_buffer->GetLength(), indexBuffer.m_buffer->GetSize(), indexBuffer.m_buffer->GetUsage());
			m_buffer->AddResourceReference();
			m_buffer->SetPersistent(false);
			m_buffer->CopyContent(*indexBuffer.m_buffer);
		}
		else
		{
			m_buffer = indexBuffer.m_buffer;
			m_buffer->AddResourceReference();
		}
	}
	else
		m_buffer = nullptr;
}

NzIndexBuffer::~NzIndexBuffer()
{
	if (m_buffer)
		m_buffer->RemoveResourceReference();
}

bool NzIndexBuffer::Fill(const void* data, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to fill sequential buffer");
		return false;
	}

	if (offset+length > m_indexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return false;
	}
	#endif

	return m_buffer->Fill(data, m_startIndex+offset, length);
}

NzBuffer* NzIndexBuffer::GetBuffer() const
{
	return m_buffer;
}

nzUInt8 NzIndexBuffer::GetIndexSize() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer: Buffer has no index size");
		return 0;
	}
	#endif

	return m_buffer->GetTypeSize();
}

unsigned int NzIndexBuffer::GetIndexCount() const
{
	return m_indexCount;
}

unsigned int NzIndexBuffer::GetStartIndex() const
{
	return m_startIndex;
}

bool NzIndexBuffer::IsHardware() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Sequential index buffer is neither hardware or software");
		return false;
	}
	#endif

	return m_buffer->IsHardware();
}

bool NzIndexBuffer::IsSequential() const
{
	return m_buffer == nullptr;
}

void* NzIndexBuffer::Lock(nzBufferLock lock, unsigned int offset, unsigned int length)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to lock sequential index buffer");
		return nullptr;
	}

	if (offset+length > m_indexCount)
	{
		NazaraError("Exceeding virtual buffer size");
		return nullptr;
	}
	#endif

	return m_buffer->Lock(lock, m_startIndex+offset, (length) ? length : m_indexCount-offset);
}

bool NzIndexBuffer::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_buffer)
	{
		NazaraError("Impossible to unlock sequential index buffer");
		return false;
	}
	#endif

	return m_buffer->Unlock();
}
