// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <map>
#include <tuple>

class NzCamera;
class NzMaterial;
class NzSkeletalMesh;
class NzStaticMesh;

class NAZARA_API NzForwardRenderQueue : public NzAbstractRenderQueue, NzResourceListener
{
	friend class NzForwardRenderTechnique;

	public:
		NzForwardRenderQueue() = default;
		~NzForwardRenderQueue();

		void AddDrawable(const NzDrawable* drawable);
		void AddLight(const NzLight* light);
		void AddModel(const NzModel* model);
		void AddSprite(const NzSprite* sprite);

		void Clear(bool fully);

		void Sort(const NzCamera& camera);

	private:
		bool OnResourceDestroy(const NzResource* resource, int index) override;

		struct SkeletalData
		{
			///TODO
			NzMatrix4f transformMatrix;
		};

		struct StaticData
		{
			NzMatrix4f transformMatrix;
		};

		struct TransparentModel
		{
			NzMatrix4f transformMatrix;
			NzSpheref boundingSphere;
			NzMaterial* material;
		};

		struct TransparentSkeletalModel : public TransparentModel
		{
			///TODO
		};

		struct TransparentStaticModel : public TransparentModel
		{
			NzStaticMesh* mesh;
		};


		struct BatchedModelMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedSpriteMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedSkeletalMeshComparator
		{
			bool operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2);
		};

		struct BatchedStaticMeshComparator
		{
			bool operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2);
		};

		typedef std::map<const NzSkeletalMesh*, std::vector<SkeletalData>, BatchedSkeletalMeshComparator> BatchedSkeletalMeshContainer;
		typedef std::map<const NzStaticMesh*, std::pair<NzSpheref, std::vector<StaticData>>, BatchedStaticMeshComparator> BatchedStaticMeshContainer;
		typedef std::map<const NzMaterial*, std::tuple<bool, bool, BatchedSkeletalMeshContainer, BatchedStaticMeshContainer>, BatchedModelMaterialComparator> BatchedModelContainer;
		typedef std::map<const NzMaterial*, std::vector<const NzSprite*>> BatchedSpriteContainer;
		typedef std::vector<const NzLight*> LightContainer;
		typedef std::vector<std::pair<unsigned int, bool>> TransparentModelContainer;

		BatchedModelContainer opaqueModels;
		BatchedSpriteContainer sprites;
		TransparentModelContainer transparentsModels;
		std::vector<TransparentSkeletalModel> transparentSkeletalModels;
		std::vector<TransparentStaticModel> transparentStaticModels;
		std::vector<const NzDrawable*> otherDrawables;
		LightContainer directionnalLights;
		LightContainer lights;
};

#endif // NAZARA_FORWARDRENDERQUEUE_HPP
