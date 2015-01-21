// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEBUGDRAWER_HPP
#define NAZARA_DEBUGDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Utility/StaticMesh.hpp>

class NzSkeleton;

class NAZARA_API NzDebugDrawer
{
	public:
		static void Draw(const NzBoundingVolumef& volume);
		static void Draw(const NzBoxf& box);
		static void Draw(const NzBoxi& box);
		static void Draw(const NzBoxui& box);
		static void Draw(const NzFrustumf& frustum);
		static void Draw(const NzOrientedBoxf& orientedBox);
		static void Draw(const NzSkeleton* skeleton);
		static void Draw(const NzVector3f& position, float size = 0.1f);
		static void DrawBinormals(const NzStaticMesh* subMesh);
		static void DrawCone(const NzVector3f& origin, const NzQuaternionf& rotation, float angle, float length);
		static void DrawLine(const NzVector3f& p1, const NzVector3f& p2);
		static void DrawPoints(const NzVector3f* ptr, unsigned int pointCount);
		static void DrawNormals(const NzStaticMesh* subMesh);
		static void DrawTangents(const NzStaticMesh* subMesh);

		static void EnableDepthBuffer(bool depthBuffer);

		static float GetLineWidth();
		static float GetPointSize();
		static NzColor GetPrimaryColor();
		static NzColor GetSecondaryColor();

		static bool Initialize();
		static bool IsDepthBufferEnabled();

		static void SetLineWidth(float width);
		static void SetPointSize(float size);
		static void SetPrimaryColor(const NzColor& color);
		static void SetSecondaryColor(const NzColor& color);

		static void Uninitialize();
};

#endif // NAZARA_DEBUG_DRAWER_HPP
