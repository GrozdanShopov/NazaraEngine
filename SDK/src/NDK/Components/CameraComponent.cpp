// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/CameraComponent.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::CameraComponent
	* \brief NDK class that represents the component for camera
	*/

	/*!
	* \brief Applys the view of the camera
	*
	* \remark Produces a NazaraAssert if the camera has no target
	*/

	void CameraComponent::ApplyView() const
	{
		NazaraAssert(m_target, "CameraComponent has no target");

		EnsureProjectionMatrixUpdate();
		EnsureViewMatrixUpdate();
		EnsureViewportUpdate();

		Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, m_projectionMatrix);
		Nz::Renderer::SetMatrix(Nz::MatrixType_View, m_viewMatrix);
		Nz::Renderer::SetTarget(m_target);
		Nz::Renderer::SetViewport(m_viewport);
	}

	/*!
	* \brief Gets the eye position of the camera
	*
	* \remark Produces a NazaraAssert if entity is invalid or has no NodeComponent
	*/

	Nz::Vector3f CameraComponent::GetEyePosition() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "CameraComponent requires NodeComponent");

		return m_entity->GetComponent<NodeComponent>().GetPosition();
	}

	/*!
	* \brief Gets the forward direction of the camera
	*
	* \remark Produces a NazaraAssert if entity is invalid or has no NodeComponent
	*/

	Nz::Vector3f CameraComponent::GetForward() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "CameraComponent requires NodeComponent");

		return m_entity->GetComponent<NodeComponent>().GetForward();
	}

	/*!
	* \brief Sets the layer of the camera in case of multiples fields
	*
	* \param layer Layer of the camera
	*/

	void CameraComponent::SetLayer(unsigned int layer)
	{
		m_layer = layer;

		m_entity->Invalidate(); // Invalidate the entity to make it passes through RenderSystem validation
	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void CameraComponent::OnAttached()
	{
		if (m_entity->HasComponent<NodeComponent>())
			m_nodeInvalidationSlot.Connect(m_entity->GetComponent<NodeComponent>().OnNodeInvalidation, this, &CameraComponent::OnNodeInvalidated);

		InvalidateViewMatrix();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void CameraComponent::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			NodeComponent& nodeComponent = static_cast<NodeComponent&>(component);
			m_nodeInvalidationSlot.Connect(nodeComponent.OnNodeInvalidation, this, &CameraComponent::OnNodeInvalidated);

			InvalidateViewMatrix();
		}
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void CameraComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<NodeComponent>(component))
		{
			m_nodeInvalidationSlot.Disconnect();

			InvalidateViewMatrix();
		}
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void CameraComponent::OnDetached()
	{
		m_nodeInvalidationSlot.Disconnect();

		InvalidateViewMatrix();
	}

	/*!
	* \brief Operation to perform when the node is invalidated
	*
	* \param node Pointer to the node
	*/

	void CameraComponent::OnNodeInvalidated(const Nz::Node* node)
	{
		NazaraUnused(node);

		// Our view matrix depends on NodeComponent position/rotation
		InvalidateViewMatrix();
	}

	/*!
	* \brief Operation to perform when the render target is released
	*
	* \param renderTarget Pointer to the RenderTarget
	*/

	void CameraComponent::OnRenderTargetRelease(const Nz::RenderTarget* renderTarget)
	{
		if (renderTarget == m_target)
			m_target = nullptr;
		else
			NazaraInternalError("Not listening to " + Nz::String::Pointer(renderTarget));
	}

	/*!
	* \brief Operation to perform when the render target has its size changed
	*
	* \param renderTarget Pointer to the RenderTarget
	*/

	void CameraComponent::OnRenderTargetSizeChange(const Nz::RenderTarget* renderTarget)
	{
		if (renderTarget == m_target)
			InvalidateViewport();
		else
			NazaraInternalError("Not listening to " + Nz::String::Pointer(renderTarget));
	}

	/*!
	* \brief Updates the frustum of the camera
	*/

	void CameraComponent::UpdateFrustum() const
	{
		EnsureProjectionMatrixUpdate();
		EnsureViewMatrixUpdate();

		// Extract the frustum from the view and projection matrices
		m_frustum.Extract(m_viewMatrix, m_projectionMatrix);
		m_frustumUpdated = true;
	}

	/*!
	* \brief Updates the project matrix of the camera
	*/

	void CameraComponent::UpdateProjectionMatrix() const
	{
		switch (m_projectionType)
		{
			case Nz::ProjectionType_Orthogonal:
				if (m_size.x <= 0.f || m_size.y <= 0.f)
				{
					EnsureViewportUpdate();

					m_projectionMatrix.MakeOrtho(0.f, static_cast<float>(m_viewport.width), 0.f, static_cast<float>(m_viewport.height), m_zNear, m_zFar);
				}
				else
					m_projectionMatrix.MakeOrtho(0.f, m_size.x, 0.f, m_size.y, m_zNear, m_zFar);
				break;

			case Nz::ProjectionType_Perspective:
				EnsureViewportUpdate(); // Can affect aspect ratio

				m_projectionMatrix.MakePerspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
				break;
		}

		m_projectionMatrixUpdated = true;
	}

	/*!
	* \brief Updates the view matrix of the camera
	*
	* \remark Produces a NazaraAssert if entity is invalid or has no NodeComponent
	*/

	void CameraComponent::UpdateViewMatrix() const
	{
		NazaraAssert(m_entity && m_entity->HasComponent<NodeComponent>(), "CameraComponent requires NodeComponent");

		NodeComponent& nodeComponent = m_entity->GetComponent<NodeComponent>();

		// Build the view matrix using the NodeComponent position/rotation
		m_viewMatrix.MakeViewMatrix(nodeComponent.GetPosition(Nz::CoordSys_Global), nodeComponent.GetRotation(Nz::CoordSys_Global));
		m_viewMatrixUpdated = true;
	}

	/*!
	* \brief Updates the view port of the camera
	*
	* \remark Produces a NazaraAssert if entity has no target
	*/

	void CameraComponent::UpdateViewport() const
	{
		NazaraAssert(m_target, "CameraComponent has no target");

		unsigned int targetWidth = m_target->GetWidth();
		unsigned int targetHeight = std::max(m_target->GetHeight(), 1U); // Let's make sure we won't divide by zero

		// Our target region is expressed as % of the viewport dimensions, let's compute it in pixels
		Nz::Rectf fViewport(m_targetRegion);
		fViewport.x *= targetWidth;
		fViewport.y *= targetHeight;
		fViewport.width *= targetWidth;
		fViewport.height *= targetHeight;

		// Compute the new aspect ratio, if it's different we need to invalidate the projection matrix
		float aspectRatio = fViewport.width/fViewport.height;
		if (!Nz::NumberEquals(m_aspectRatio, aspectRatio, 0.001f))
		{
			m_aspectRatio = aspectRatio;

			if (m_projectionType == Nz::ProjectionType_Perspective)
				InvalidateProjectionMatrix();
		}

		// Convert it back to int
		m_viewport.Set(fViewport);
		m_viewportUpdated = true;
	}

	ComponentIndex CameraComponent::componentIndex;
}
