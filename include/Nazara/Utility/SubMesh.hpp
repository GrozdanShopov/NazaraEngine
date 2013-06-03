// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESH_HPP
#define NAZARA_SUBMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NzMesh;
class NzSubMesh;

using NzSubMeshConstRef = NzResourceRef<const NzSubMesh>;
using NzSubMeshRef = NzResourceRef<NzSubMesh>;

class NAZARA_API NzSubMesh : public NzResource
{
	friend NzMesh;

	public:
		NzSubMesh(const NzMesh* parent);
		virtual ~NzSubMesh();

		void GenerateNormals();
		void GenerateNormalsAndTangents();
		void GenerateTangents();

		virtual const NzBoxf& GetAABB() const = 0;
		virtual nzAnimationType GetAnimationType() const = 0;
		virtual const NzIndexBuffer* GetIndexBuffer() const = 0;
		unsigned int GetMaterialIndex() const;
		const NzMesh* GetParent() const;
		nzPrimitiveMode GetPrimitiveMode() const;
		unsigned int GetTriangleCount() const;
		virtual unsigned int GetVertexCount() const = 0;

		virtual bool IsAnimated() const = 0;

		void SetMaterialIndex(unsigned int matIndex);
		void SetPrimitiveMode(nzPrimitiveMode mode);

	protected:
		nzPrimitiveMode m_primitiveMode;
		const NzMesh* m_parent;
		unsigned int m_matIndex;
};

#endif // NAZARA_SUBMESH_HPP
