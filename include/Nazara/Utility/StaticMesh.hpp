// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STATICMESH_HPP
#define NAZARA_STATICMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzStaticMesh;

using NzStaticMeshConstRef = NzObjectRef<const NzStaticMesh>;
using NzStaticMeshRef = NzObjectRef<NzStaticMesh>;

class NAZARA_UTILITY_API NzStaticMesh final : public NzSubMesh
{
	public:
		NzStaticMesh(const NzMesh* parent);
		~NzStaticMesh();

		void Center();

		bool Create(NzVertexBuffer* vertexBuffer);
		void Destroy();

		bool GenerateAABB();

		const NzBoxf& GetAABB() const override;
		nzAnimationType GetAnimationType() const final;
		const NzIndexBuffer* GetIndexBuffer() const override;
		NzVertexBuffer* GetVertexBuffer();
		const NzVertexBuffer* GetVertexBuffer() const;
		unsigned int GetVertexCount() const override;

		bool IsAnimated() const final;
		bool IsValid() const;

		void SetAABB(const NzBoxf& aabb);
		void SetIndexBuffer(const NzIndexBuffer* indexBuffer);

		template<typename... Args> static NzStaticMeshRef New(Args&&... args);

		// Signals:
		NazaraSignal(OnStaticMeshDestroy, const NzStaticMesh* /*staticMesh*/);
		NazaraSignal(OnStaticMeshRelease, const NzStaticMesh* /*staticMesh*/);

	private:
		NzBoxf m_aabb;
		NzIndexBufferConstRef m_indexBuffer = nullptr;
		NzVertexBufferRef m_vertexBuffer = nullptr;
};

#include <Nazara/Utility/StaticMesh.inl>

#endif // NAZARA_STATICMESH_HPP
