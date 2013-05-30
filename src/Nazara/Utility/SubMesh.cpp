// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzSubMesh::NzSubMesh(const NzMesh* parent) :
NzResource(false), // Un SubMesh n'est pas persistant par défaut
m_primitiveMode(nzPrimitiveMode_TriangleList),
m_parent(parent),
m_matIndex(0)
{
}

NzSubMesh::~NzSubMesh() = default;

void NzSubMesh::GenerateNormals()
{
	NzVertexMapper mapper1(this);
	unsigned int vertexCount = mapper1.GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
		mapper1.SetNormal(i, NzVector3f::Zero());

	mapper1.Unmap();

	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = iterator.GetPosition(0);

		NzVector3f dv[2];
		dv[0] = iterator.GetPosition(1) - pos0;
		dv[1] = iterator.GetPosition(2) - pos0;

		NzVector3f normal = dv[0].CrossProduct(dv[1]);

		for (unsigned int i = 0; i < 3; ++i)
			iterator.SetNormal(i, iterator.GetNormal(i) + normal);
	}
	while (iterator.Advance());

	NzVertexMapper mapper2(this);
	for (unsigned int i = 0; i < vertexCount; ++i)
		mapper2.SetNormal(i, NzVector3f::Normalize(mapper2.GetNormal(i)));
}

void NzSubMesh::GenerateNormalsAndTangents()
{
	NzVertexMapper mapper1(this);
	unsigned int vertexCount = mapper1.GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		mapper1.SetNormal(i, NzVector3f::Zero());
		mapper1.SetTangent(i, NzVector3f::Zero());
	}

	mapper1.Unmap();

	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = iterator.GetPosition(0);
		NzVector2f uv0 = iterator.GetTexCoord(0);

		NzVector3f dv[2];
		dv[0] = iterator.GetPosition(1) - pos0;
		dv[1] = iterator.GetPosition(2) - pos0;

		NzVector3f normal = dv[0].CrossProduct(dv[1]);

		NzVector2f duv[2];
		duv[0] = iterator.GetTexCoord(1) - uv0;
		duv[1] = iterator.GetTexCoord(2) - uv0;

		float coef = 1.f / (duv[0].x*duv[1].y - duv[1].x*duv[0].y);

		NzVector3f tangent;
		tangent.x = coef * (dv[0].x*duv[1].y + dv[1].x*(-duv[0].y));
		tangent.y = coef * (dv[0].y*duv[1].y + dv[1].y*(-duv[0].y));
		tangent.z = coef * (dv[0].z*duv[1].y + dv[1].z*(-duv[0].y));

		for (unsigned int i = 0; i < 3; ++i)
		{
			iterator.SetNormal(i, iterator.GetNormal(i) + normal);
			iterator.SetTangent(i, iterator.GetTangent(i) + tangent);
		}
	}
	while (iterator.Advance());

	NzVertexMapper mapper2(this);
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		mapper2.SetNormal(i, NzVector3f::Normalize(mapper2.GetNormal(i)));
		mapper2.SetTangent(i, NzVector3f::Normalize(mapper2.GetTangent(i)));
	}
}

void NzSubMesh::GenerateTangents()
{
	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = iterator.GetPosition(0);
		NzVector2f uv0 = iterator.GetTexCoord(0);

		NzVector3f dv[2];
		dv[0] = iterator.GetPosition(1) - pos0;
		dv[1] = iterator.GetPosition(2) - pos0;

		NzVector2f duv[2];
		duv[0] = iterator.GetTexCoord(1) - uv0;
		duv[1] = iterator.GetTexCoord(2) - uv0;

		float ds[2];
		ds[0] = iterator.GetTexCoord(1).x - uv0.x;
		ds[1] = iterator.GetTexCoord(2).x - uv0.x;

		NzVector3f ppt;
		ppt.x = ds[0]*dv[1].x - dv[0].x*ds[1];
		ppt.y = ds[0]*dv[1].y - dv[0].y*ds[1];
		ppt.z = ds[0]*dv[1].z - dv[0].z*ds[1];
		ppt.Normalize();

		for (unsigned int i = 0; i < 3; ++i)
		{
			NzVector3f normal = iterator.GetNormal(i);
			float d = ppt.DotProduct(normal);

			NzVector3f tangent = ppt - (d * normal);

			iterator.SetTangent(i, tangent);
		}
	}
	while (iterator.Advance());
}

const NzMesh* NzSubMesh::GetParent() const
{
	return m_parent;
}

nzPrimitiveMode NzSubMesh::GetPrimitiveMode() const
{
	return m_primitiveMode;
}

unsigned int NzSubMesh::GetTriangleCount() const
{
	const NzIndexBuffer* indexBuffer = GetIndexBuffer();
	unsigned int indexCount;
	if (indexBuffer)
		indexCount = indexBuffer->GetIndexCount();
	else
		indexCount = GetVertexCount();

	switch (m_primitiveMode)
	{
		case nzPrimitiveMode_LineList:
		case nzPrimitiveMode_LineStrip:
		case nzPrimitiveMode_PointList:
			return 0;

		case nzPrimitiveMode_TriangleFan:
			return (indexCount - 1) / 2;

		case nzPrimitiveMode_TriangleList:
			return indexCount / 3;

		case nzPrimitiveMode_TriangleStrip:
			return indexCount - 2;
	}

	NazaraError("Primitive mode not handled (0x" + NzString::Number(m_primitiveMode, 16) + ')');
	return 0;
}

unsigned int NzSubMesh::GetMaterialIndex() const
{
	return m_matIndex;
}

void NzSubMesh::SetPrimitiveMode(nzPrimitiveMode mode)
{
	m_primitiveMode = mode;
}

void NzSubMesh::SetMaterialIndex(unsigned int matIndex)
{
	m_matIndex = matIndex;
}
