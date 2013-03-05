// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp> // NzMeshVertex
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

class NzVertexMapperImpl
{
	public:
		NzVertexMapperImpl() = default;
		virtual ~NzVertexMapperImpl() = default;

		virtual NzVector3f GetNormal(unsigned int i) const = 0;
		virtual NzVector3f GetPosition(unsigned int i) const = 0;
		virtual NzVector3f GetTangent(unsigned int i) const = 0;
		virtual NzVector2f GetTexCoords(unsigned int i) const = 0;
		virtual unsigned int GetVertexCount() const = 0;

		virtual void SetNormal(unsigned int i, const NzVector3f& normal) = 0;
		virtual void SetPosition(unsigned int i, const NzVector3f& position) = 0;
		virtual void SetTangent(unsigned int i, const NzVector3f& tangent) = 0;
		virtual void SetTexCoords(unsigned int i, const NzVector2f& texCoords) = 0;
};

namespace
{
	class SubMeshVertexMapper : public NzVertexMapperImpl
	{
		public:
			SubMeshVertexMapper(NzSubMesh* subMesh) :
			m_subMesh(subMesh)
			{
				#if NAZARA_UTILITY_SAFE
				if (!m_subMesh)
				{
					NazaraError("Invalid subMesh"); ///TODO: Unexpected
					return;
				}
				#endif
			}

			virtual ~SubMeshVertexMapper() noexcept
			{
				m_subMesh->Finish();
			}

		protected:
			NzSubMesh* m_subMesh;
	};

	class SkeletalMeshVertexMapper : public SubMeshVertexMapper
	{
		public:
			SkeletalMeshVertexMapper(NzSkeletalMesh* subMesh) :
			SubMeshVertexMapper(subMesh),
			m_mesh(subMesh)
			{
				m_vertices = reinterpret_cast<NzMeshVertex*>(m_mesh->GetBindPoseBuffer());

				m_mesh->AddResourceReference();
			}

			virtual ~SkeletalMeshVertexMapper() noexcept
			{
				m_mesh->RemoveResourceReference();
			}

			NzVector3f GetNormal(unsigned int i) const
			{
				return m_vertices[i].normal;
			}

			NzVector3f GetPosition(unsigned int i) const
			{
				return m_vertices[i].position;
			}

			NzVector3f GetTangent(unsigned int i) const
			{
				return m_vertices[i].tangent;
			}

			NzVector2f GetTexCoords(unsigned int i) const
			{
				return m_vertices[i].uv;
			}

			unsigned int GetVertexCount() const
			{
				return m_mesh->GetVertexCount();
			}

			void SetNormal(unsigned int i, const NzVector3f& normal)
			{
				m_vertices[i].normal = normal;
			}

			void SetPosition(unsigned int i, const NzVector3f& position)
			{
				m_vertices[i].position = position;
			}

			void SetTangent(unsigned int i, const NzVector3f& tangent)
			{
				m_vertices[i].tangent = tangent;
			}

			void SetTexCoords(unsigned int i, const NzVector2f& texCoords)
			{
				m_vertices[i].uv = texCoords;
			}

		private:
			NzMeshVertex* m_vertices;
			NzSkeletalMesh* m_mesh;
	};

	class StaticMeshVertexMapper : public SubMeshVertexMapper
	{
		public:
			StaticMeshVertexMapper(NzStaticMesh* subMesh) :
			SubMeshVertexMapper(subMesh),
			m_vertexMapper(subMesh->GetVertexBuffer(), nzBufferAccess_ReadWrite)
			{
				m_vertices = reinterpret_cast<NzMeshVertex*>(m_vertexMapper.GetPointer());
			}

			virtual ~StaticMeshVertexMapper() noexcept
			{
			}

			NzVector3f GetNormal(unsigned int i) const
			{
				return m_vertices[i].normal;
			}

			NzVector3f GetPosition(unsigned int i) const
			{
				return m_vertices[i].position;
			}

			NzVector3f GetTangent(unsigned int i) const
			{
				return m_vertices[i].tangent;
			}

			NzVector2f GetTexCoords(unsigned int i) const
			{
				return m_vertices[i].uv;
			}

			unsigned int GetVertexCount() const
			{
				return m_vertexMapper.GetBuffer()->GetVertexCount();
			}

			void SetNormal(unsigned int i, const NzVector3f& normal)
			{
				m_vertices[i].normal = normal;
			}

			void SetPosition(unsigned int i, const NzVector3f& position)
			{
				m_vertices[i].position = position;
			}

			void SetTangent(unsigned int i, const NzVector3f& tangent)
			{
				m_vertices[i].tangent = tangent;
			}

			void SetTexCoords(unsigned int i, const NzVector2f& texCoords)
			{
				m_vertices[i].uv = texCoords;
			}

		private:
			NzBufferMapper<NzVertexBuffer> m_vertexMapper;
			NzMeshVertex* m_vertices;
	};
}

NzVertexMapper::NzVertexMapper(NzSubMesh* subMesh)
{
	switch (subMesh->GetAnimationType())
	{
		case nzAnimationType_Skeletal:
			m_impl = new SkeletalMeshVertexMapper(static_cast<NzSkeletalMesh*>(subMesh));
			break;

		case nzAnimationType_Static:
			m_impl = new StaticMeshVertexMapper(static_cast<NzStaticMesh*>(subMesh));
			break;
	}

	#ifdef NAZARA_DEBUG
	if (!m_impl)
		NazaraInternalError("No impl"); ///TODO: Internal, Unexpected
	#endif
}

NzVertexMapper::~NzVertexMapper()
{
	delete m_impl;
}

NzVector3f NzVertexMapper::GetNormal(unsigned int i) const
{
	return m_impl->GetNormal(i);
}

NzVector3f NzVertexMapper::GetPosition(unsigned int i) const
{
	return m_impl->GetPosition(i);
}

NzVector3f NzVertexMapper::GetTangent(unsigned int i) const
{
	return m_impl->GetTangent(i);
}

NzVector2f NzVertexMapper::GetTexCoords(unsigned int i) const
{
	return m_impl->GetTexCoords(i);
}

unsigned int NzVertexMapper::GetVertexCount()
{
	return m_impl->GetVertexCount();
}

void NzVertexMapper::SetNormal(unsigned int i, const NzVector3f& normal)
{
	m_impl->SetNormal(i, normal);
}

void NzVertexMapper::SetPosition(unsigned int i, const NzVector3f& position)
{
	m_impl->SetPosition(i, position);
}

void NzVertexMapper::SetTangent(unsigned int i, const NzVector3f& tangent)
{
	m_impl->SetTangent(i, tangent);
}

void NzVertexMapper::SetTexCoords(unsigned int i, const NzVector2f& texCoords)
{
	m_impl->SetTexCoords(i, texCoords);
}

void NzVertexMapper::Unmap()
{
	if (m_impl)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}
