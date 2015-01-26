// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_UTILITY_HPP
#define NAZARA_ALGORITHM_UTILITY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>

struct NzSkinningData
{
	const NzJoint* joints;
	const NzSkeletalMeshVertex* inputVertex;
	NzMeshVertex* outputVertex;
};

struct NzVertexPointers
{
	NzSparsePtr<NzVector3f> normalPtr;
	NzSparsePtr<NzVector3f> positionPtr;
	NzSparsePtr<NzVector3f> tangentPtr;
	NzSparsePtr<NzVector2f> uvPtr;
};

NAZARA_API NzBoxf NzComputeAABB(NzSparsePtr<const NzVector3f> positionPtr, unsigned int vertexCount);
NAZARA_API void NzComputeBoxIndexVertexCount(const NzVector3ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API unsigned int NzComputeCacheMissCount(NzIndexIterator indices, unsigned int indexCount);
NAZARA_API void NzComputeConeIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeCubicSphereIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputePlaneIndexVertexCount(const NzVector2ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, unsigned int* indexCount, unsigned int* vertexCount);

NAZARA_API void NzGenerateBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateCone(float length, float radius, unsigned int subdivision, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGeneratePlane(const NzVector2ui& subdivision, const NzVector2f& size, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& matrix, const NzRectf& textureCoords, NzVertexPointers vertexPointers, NzIndexIterator indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);

NAZARA_API void NzOptimizeIndices(NzIndexIterator indices, unsigned int indexCount);

NAZARA_API void NzSkinPosition(const NzSkinningData& data, unsigned int startVertex, unsigned int vertexCount);
NAZARA_API void NzSkinPositionNormal(const NzSkinningData& data, unsigned int startVertex, unsigned int vertexCount);
NAZARA_API void NzSkinPositionNormalTangent(const NzSkinningData& data, unsigned int startVertex, unsigned int vertexCount);

NAZARA_API void NzTransformVertices(NzVertexPointers vertexPointers, unsigned int vertexCount, const NzMatrix4f& matrix);

#endif // NAZARA_ALGORITHM_UTILITY_HPP
