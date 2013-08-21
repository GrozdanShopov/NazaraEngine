// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIEW_HPP
#define NAZARA_VIEW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>

class NAZARA_API NzView : public NzAbstractViewer, public NzNode, NzRenderTarget::Listener
{
	public:
		NzView();
		~NzView();

		void EnsureFrustumUpdate() const;
		void EnsureProjectionMatrixUpdate() const;
		void EnsureViewMatrixUpdate() const;
		void EnsureViewportUpdate() const;

		float GetAspectRatio() const;
		NzVector3f GetEyePosition() const;
		const NzFrustumf& GetFrustum() const;
		const NzMatrix4f& GetProjectionMatrix() const;
		const NzRenderTarget* GetTarget() const;
		const NzRectf& GetTargetRegion() const;
		const NzMatrix4f& GetViewMatrix() const;
		const NzRectui& GetViewport() const;
		float GetZFar() const;
		float GetZNear() const;

		void SetTarget(const NzRenderTarget* renderTarget);
		void SetTarget(const NzRenderTarget& renderTarget);
		void SetTargetRegion(const NzRectf& region);
		void SetViewport(const NzRectui& viewport);
		void SetZFar(float zFar);
		void SetZNear(float zNear);

	private:
		void ApplyView() const override;
		void Invalidate() override;

		void OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata) override;
		bool OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata) override;

		void UpdateFrustum() const;
		void UpdateProjectionMatrix() const;
		void UpdateViewMatrix() const;
		void UpdateViewport() const;

		mutable NzFrustumf m_frustum;
		mutable NzMatrix4f m_projectionMatrix;
		mutable NzMatrix4f m_viewMatrix;
		NzRectf m_targetRegion;
		mutable NzRectui m_viewport;
		const NzRenderTarget* m_target;
		mutable bool m_frustumUpdated;
		mutable bool m_projectionMatrixUpdated;
		mutable bool m_viewMatrixUpdated;
		mutable bool m_viewportUpdated;
		float m_zFar;
		float m_zNear;
};

#endif // NAZARA_VIEW_HPP
