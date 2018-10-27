// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MESH_HPP
#define NAZARA_MESH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	struct NAZARA_UTILITY_API MeshParams : ResourceParameters
	{
		MeshParams();

		BufferUsageFlags indexBufferFlags = 0;      ///< Buffer usage flags used to build the index buffers
		BufferUsageFlags vertexBufferFlags = 0;     ///< Buffer usage flags used to build the vertex buffers
		Matrix4f matrix = Matrix4f::Identity();     ///< A matrix which will transform every vertex position
		DataStorage storage = DataStorage_Hardware; ///< The place where the buffers will be allocated
		Vector2f texCoordOffset = {0.f, 0.f};       ///< Offset to apply on the texture coordinates (not scaled)
		Vector2f texCoordScale  = {1.f, 1.f};       ///< Scale to apply on the texture coordinates
		bool animated = true;                       ///< If true, will load an animated version of the model if possible
		bool center = false;                        ///< If true, will center the mesh vertices around the origin
		#ifndef NAZARA_DEBUG
		bool optimizeIndexBuffers = true;           ///< Optimize the index buffers after loading, improve cache locality (and thus rendering speed) but increase loading time.
		#else
		bool optimizeIndexBuffers = false;          ///< Since this optimization take a lot of time, especially in debug mode, don't enable it by default in debug.
		#endif

		/* The declaration must have a Vector3f position component enabled
		 * If the declaration has a Vector2f UV component enabled, UV are generated
		 * If the declaration has a Vector3f Normals component enabled, Normals are generated.
		 * If the declaration has a Vector3f Tangents component enabled, Tangents are generated.
		 */
		VertexDeclaration* vertexDeclaration = VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent);

		bool IsValid() const;
	};

	class Mesh;
	struct Primitive;
	class PrimitiveList;
	class SubMesh;

	using MeshVertex = VertexStruct_XYZ_Normal_UV_Tangent;
	using SkeletalMeshVertex = VertexStruct_XYZ_Normal_UV_Tangent_Skinning;

	using MeshConstRef = ObjectRef<const Mesh>;
	using MeshLibrary = ObjectLibrary<Mesh>;
	using MeshLoader = ResourceLoader<Mesh, MeshParams>;
	using MeshManager = ResourceManager<Mesh, MeshParams>;
	using MeshRef = ObjectRef<Mesh>;
	using MeshSaver = ResourceSaver<Mesh, MeshParams>;

	struct MeshImpl;

	class NAZARA_UTILITY_API Mesh : public RefCounted, public Resource
	{
		friend MeshLibrary;
		friend MeshLoader;
		friend MeshManager;
		friend MeshSaver;
		friend class Utility;

		public:
			inline Mesh();
			Mesh(const Mesh&) = delete;
			Mesh(Mesh&&) = delete;
			inline ~Mesh();

			void AddSubMesh(SubMesh* subMesh);
			void AddSubMesh(const String& identifier, SubMesh* subMesh);

			SubMesh* BuildSubMesh(const Primitive& primitive, const MeshParams& params = MeshParams());
			void BuildSubMeshes(const PrimitiveList& list, const MeshParams& params = MeshParams());

			bool CreateSkeletal(UInt32 jointCount);
			bool CreateStatic();
			void Destroy();

			void GenerateNormals();
			void GenerateNormalsAndTangents();
			void GenerateTangents();

			const Boxf& GetAABB() const;
			String GetAnimation() const;
			AnimationType GetAnimationType() const;
			UInt32 GetJointCount() const;
			ParameterList& GetMaterialData(UInt32 index);
			const ParameterList& GetMaterialData(UInt32 index) const;
			UInt32 GetMaterialCount() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;
			SubMesh* GetSubMesh(const String& identifier);
			SubMesh* GetSubMesh(UInt32 index);
			const SubMesh* GetSubMesh(const String& identifier) const;
			const SubMesh* GetSubMesh(UInt32 index) const;
			UInt32 GetSubMeshCount() const;
			UInt32 GetSubMeshIndex(const String& identifier) const;
			UInt32 GetTriangleCount() const;
			UInt32 GetVertexCount() const;

			bool HasSubMesh(const String& identifier) const;
			bool HasSubMesh(UInt32 index = 0) const;

			void InvalidateAABB() const;

			bool IsAnimable() const;
			bool IsValid() const;

			void Recenter();

			void RemoveSubMesh(const String& identifier);
			void RemoveSubMesh(UInt32 index);

			bool SaveToFile(const String& filePath, const MeshParams& params = MeshParams());
			bool SaveToStream(Stream& stream, const String& format, const MeshParams& params = MeshParams());

			void SetAnimation(const String& animationPath);
			void SetMaterialCount(UInt32 matCount);
			void SetMaterialData(UInt32 matIndex, ParameterList data);

			void Transform(const Matrix4f& matrix);

			Mesh& operator=(const Mesh&) = delete;
			Mesh& operator=(Mesh&&) = delete;

			static MeshRef LoadFromFile(const String& filePath, const MeshParams& params = MeshParams());
			static MeshRef LoadFromMemory(const void* data, std::size_t size, const MeshParams& params = MeshParams());
			static MeshRef LoadFromStream(Stream& stream, const MeshParams& params = MeshParams());

			template<typename... Args> static MeshRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnMeshDestroy, const Mesh* /*mesh*/);
			NazaraSignal(OnMeshInvalidateAABB, const Mesh* /*mesh*/);
			NazaraSignal(OnMeshRelease, const Mesh* /*mesh*/);

		private:
			struct SubMeshData
			{
				SubMeshRef subMesh;

				NazaraSlot(SubMesh, OnSubMeshInvalidateAABB, onSubMeshInvalidated);
			};

			std::unordered_map<String, UInt32> m_subMeshMap;
			std::vector<ParameterList> m_materialData;
			std::vector<SubMeshData> m_subMeshes;
			AnimationType m_animationType;
			mutable Boxf m_aabb;
			Skeleton m_skeleton; // Only used by skeletal meshes
			String m_animationPath;
			mutable bool m_aabbUpdated;
			bool m_isValid;
			UInt32 m_jointCount; // Only used by skeletal meshes

			static bool Initialize();
			static void Uninitialize();

			static MeshLibrary::LibraryMap s_library;
			static MeshLoader::LoaderList s_loaders;
			static MeshManager::ManagerMap s_managerMap;
			static MeshManager::ManagerParams s_managerParameters;
			static MeshSaver::SaverList s_savers;
	};
}

#include <Nazara/Utility/Mesh.inl>

#endif // NAZARA_MESH_HPP
