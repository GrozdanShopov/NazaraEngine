// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESH_HPP
#define NAZARA_MESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>

struct NAZARA_API NzMeshParams
{
	NzMeshParams(); // Vérifie que le storage par défaut est supporté (software autrement)

	// La mise à l'échelle éventuelle que subira le mesh
	NzVector3f scale = NzVector3f::Unit();

	// Si ceci sera le stockage utilisé par les buffers
	nzUInt32 storage = nzDataStorage_Hardware;

	// Charger une version animée du mesh si possible ?
	bool animated = true;

	// Faut-il centrer le mesh autour de l'origine ?
	bool center = false;

	// Faut-il retourner les UV ?
	bool flipUVs = false;

	// Faut-il optimiser les index buffers ? (Rendu plus rapide, mais le chargement dure plus longtemps)
	bool optimizeIndexBuffers = true;

	bool IsValid() const;
};

class NzMesh;
class NzPrimitiveList;

typedef NzVertexStruct_XYZ_Normal_UV_Tangent NzMeshVertex;
typedef NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning NzSkeletalMeshVertex;

using NzMeshConstListener = NzObjectListenerWrapper<const NzMesh>;
using NzMeshConstRef = NzObjectRef<const NzMesh>;
using NzMeshLibrary = NzObjectLibrary<NzMesh>;
using NzMeshListener = NzObjectListenerWrapper<NzMesh>;
using NzMeshLoader = NzResourceLoader<NzMesh, NzMeshParams>;
using NzMeshManager = NzResourceManager<NzMesh, NzMeshParams>;
using NzMeshRef = NzObjectRef<NzMesh>;

struct NzMeshImpl;

class NAZARA_API NzMesh : public NzRefCounted, public NzResource
{
	friend NzMeshLibrary;
	friend NzMeshLoader;
	friend NzMeshManager;
	friend class NzUtility;

	public:
		NzMesh() = default;
		~NzMesh();

		void AddSubMesh(NzSubMesh* subMesh);
		void AddSubMesh(const NzString& identifier, NzSubMesh* subMesh);

		NzSubMesh* BuildSubMesh(const NzPrimitive& primitive, const NzMeshParams& params = NzMeshParams());
		void BuildSubMeshes(const NzPrimitiveList& list, const NzMeshParams& params = NzMeshParams());

		bool CreateSkeletal(unsigned int jointCount);
		bool CreateStatic();
		void Destroy();

		void GenerateNormals();
		void GenerateNormalsAndTangents();
		void GenerateTangents();

		const NzBoxf& GetAABB() const;
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
		unsigned int GetTriangleCount() const;
		unsigned int GetVertexCount() const;

		bool HasSubMesh(const NzString& identifier) const;
		bool HasSubMesh(unsigned int index = 0) const;

		void InvalidateAABB() const;

		bool IsAnimable() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzMeshParams& params = NzMeshParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMeshParams& params = NzMeshParams());
		bool LoadFromStream(NzInputStream& stream, const NzMeshParams& params = NzMeshParams());

		void Recenter();

		void RemoveSubMesh(const NzString& identifier);
		void RemoveSubMesh(unsigned int index);

		void SetAnimation(const NzString& animationPath);
		void SetMaterial(unsigned int matIndex, const NzString& materialPath);
		void SetMaterialCount(unsigned int matCount);

		void Transform(const NzMatrix4f& matrix);

		template<typename... Args> static NzMeshRef New(Args&&... args);

	private:
		NzMeshImpl* m_impl = nullptr;

		static bool Initialize();
		static void Uninitialize();

		static NzMeshLibrary::LibraryMap s_library;
		static NzMeshLoader::LoaderList s_loaders;
		static NzMeshManager::ManagerMap s_managerMap;
		static NzMeshManager::ManagerParams s_managerParameters;
};

#include <Nazara/Utility/Mesh.inl>

#endif // NAZARA_MESH_HPP
