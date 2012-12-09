// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESH_HPP
#define NAZARA_MESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AxisAlignedBox.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>

struct NAZARA_API NzMeshParams
{
	NzMeshParams(); // Vérifie que le storage indiqué un peu plus bas est supporté

	nzBufferStorage storage = nzBufferStorage_Hardware;
	bool animated = true;

	bool IsValid() const;
};

class NzAnimation;
class NzMesh;

typedef NzVertexStruct_XYZ_Normal_UV_Tangent NzMeshVertex;

using NzMeshLoader = NzResourceLoader<NzMesh, NzMeshParams>;

struct NzMeshImpl;

class NAZARA_API NzMesh : public NzResource, NzResourceListener
{
	friend NzMeshLoader;

	public:
		NzMesh() = default;
		~NzMesh();

		bool AddSubMesh(NzSubMesh* subMesh);
		bool AddSubMesh(const NzString& identifier, NzSubMesh* subMesh);

		void Animate(const NzAnimation* animation, unsigned int frameA, unsigned int frameB, float interpolation, NzSkeleton* skeleton) const;

		bool CreateKeyframe();
		bool CreateSkeletal(unsigned int jointCount);
		bool CreateStatic();
		void Destroy();

		const NzAxisAlignedBox& GetAABB() const;
		NzString GetAnimation() const;
		nzAnimationType GetAnimationType() const;
		unsigned int GetJointCount() const;
		NzString GetMaterial(unsigned int index) const;
		unsigned int GetMaterialCount() const;
		NzSkeleton* GetSkeleton();
		const NzSkeleton* GetSkeleton() const;
		NzSubMesh* GetSubMesh(const NzString& identifier);
		NzSubMesh* GetSubMesh(unsigned int index);
		const NzSubMesh* GetSubMesh(const NzString& identifier) const;
		const NzSubMesh* GetSubMesh(unsigned int index) const;
		unsigned int GetSubMeshCount() const;
		int GetSubMeshIndex(const NzString& identifier) const;
		unsigned int GetVertexCount() const;

		bool HasSubMesh(const NzString& identifier) const;
		bool HasSubMesh(unsigned int index = 0) const;

		void InvalidateAABB() const;

		bool IsAnimable() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzMeshParams& params = NzMeshParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMeshParams& params = NzMeshParams());
		bool LoadFromStream(NzInputStream& stream, const NzMeshParams& params = NzMeshParams());

		void RemoveSubMesh(const NzString& identifier);
		void RemoveSubMesh(unsigned int index);

		void SetAnimation(const NzString& animationPath);
		void SetMaterial(unsigned int matIndex, const NzString& materialPath);
		void SetMaterialCount(unsigned int matCount);

		void Skin(const NzSkeleton* skeleton) const;

		static const NzVertexDeclaration* GetDeclaration();

	private:
		void OnResourceReleased(const NzResource* resource, int index) override;

		NzMeshImpl* m_impl = nullptr;

		static NzMeshLoader::LoaderList s_loaders;
};

#endif // NAZARA_MESH_HPP
