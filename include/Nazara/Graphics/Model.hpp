// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Utility/Mesh.hpp>

namespace Nz
{
	struct NAZARA_GRAPHICS_API ModelParameters : ResourceParameters
	{
		ModelParameters();

		bool loadMaterials = true;
		MaterialParams material;
		MeshParams mesh;

		bool IsValid() const;
	};

	class Model;

	using ModelConstRef = ObjectRef<const Model>;
	using ModelLibrary = ObjectLibrary<Model>;
	using ModelLoader = ResourceLoader<Model, ModelParameters>;
	using ModelManager = ResourceManager<Model, ModelParameters>;
	using ModelRef = ObjectRef<Model>;
	using ModelSaver = ResourceSaver<Model, ModelParameters>;

	class NAZARA_GRAPHICS_API Model : public InstancedRenderable, public Resource
	{
		friend ModelLibrary;
		friend ModelLoader;
		friend ModelManager;
		friend ModelSaver;

		public:
			inline Model();
			Model(const Model& model);
			Model(Model&& model) = delete;
			virtual ~Model();

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const override;
			inline void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix, int renderOrder = 0, const Recti& scissorRect = Recti(-1, -1, -1, -1)) const;

			std::unique_ptr<InstancedRenderable> Clone() const override;

			using InstancedRenderable::GetMaterial;
			const MaterialRef& GetMaterial(const String& subMeshName) const;
			const MaterialRef& GetMaterial(std::size_t skinIndex, const String& subMeshName) const;
			Mesh* GetMesh() const;

			virtual bool IsAnimated() const;

			using InstancedRenderable::SetMaterial;
			bool SetMaterial(const String& subMeshName, MaterialRef material);
			bool SetMaterial(std::size_t skinIndex, const String& subMeshName, MaterialRef material);

			virtual void SetMesh(Mesh* mesh);

			Model& operator=(const Model& node) = default;
			Model& operator=(Model&& node) = delete;

			static ModelRef LoadFromFile(const String& filePath, const ModelParameters& params = ModelParameters());
			static ModelRef LoadFromMemory(const void* data, std::size_t size, const ModelParameters& params = ModelParameters());
			static ModelRef LoadFromStream(Stream& stream, const ModelParameters& params = ModelParameters());

			template<typename... Args> static ModelRef New(Args&&... args);

		protected:
			void MakeBoundingVolume() const override;

			MeshRef m_mesh;

			NazaraSlot(Mesh, OnMeshInvalidateAABB, m_meshAABBInvalidationSlot);

			static ModelLibrary::LibraryMap s_library;
			static ModelLoader::LoaderList s_loaders;
			static ModelManager::ManagerMap s_managerMap;
			static ModelManager::ManagerParams s_managerParameters;
			static ModelSaver::SaverList s_savers;
	};
}

#include <Nazara/Graphics/Model.inl>

#endif // NAZARA_MODEL_HPP
