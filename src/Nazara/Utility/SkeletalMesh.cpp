// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SkeletalMesh::SkeletalMesh(VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer) :
	m_aabb(Nz::Boxf::Zero()),
	m_indexBuffer(indexBuffer),
	m_vertexBuffer(vertexBuffer)
	{
		NazaraAssert(m_vertexBuffer, "Invalid vertex buffer");
	}

	SkeletalMesh::SkeletalMesh(const Mesh* /*parent*/) :
	m_aabb(Nz::Boxf::Zero())
	{
	}

	SkeletalMesh::~SkeletalMesh()
	{
		OnSkeletalMeshRelease(this);

		Destroy();
	}

	bool SkeletalMesh::Create(VertexBuffer* vertexBuffer)
	{
		Destroy();

		#if NAZARA_UTILITY_SAFE
		if (!vertexBuffer)
		{
			NazaraError("Invalid vertex buffer");
			return false;
		}
		#endif

		m_vertexBuffer = vertexBuffer;
		return true;
	}

	void SkeletalMesh::Destroy()
	{
		if (m_vertexBuffer)
		{
			OnSkeletalMeshDestroy(this);

			m_indexBuffer.Reset();
			m_vertexBuffer.Reset();
		}
	}

	const Boxf& SkeletalMesh::GetAABB() const
	{
		return m_aabb;
	}

	AnimationType SkeletalMesh::GetAnimationType() const
	{
		return AnimationType_Skeletal;
	}

	const IndexBuffer* SkeletalMesh::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}

	VertexBuffer* SkeletalMesh::GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	const VertexBuffer* SkeletalMesh::GetVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	std::size_t SkeletalMesh::GetVertexCount() const
	{
		return m_vertexBuffer->GetVertexCount();
	}

	bool SkeletalMesh::IsAnimated() const
	{
		return true;
	}

	bool SkeletalMesh::IsValid() const
	{
		return m_vertexBuffer != nullptr;
	}

	void SkeletalMesh::SetAABB(const Boxf& aabb)
	{
		m_aabb = aabb;

		OnSubMeshInvalidateAABB(this);
	}

	void SkeletalMesh::SetIndexBuffer(const IndexBuffer* indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}
}
