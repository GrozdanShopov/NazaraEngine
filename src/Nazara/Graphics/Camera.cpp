// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzCamera::NzCamera() :
m_viewport(0.f, 0.f, 1.f, 1.f),
m_target(nullptr),
m_frustumUpdated(false),
m_projectionMatrixUpdated(false),
m_viewMatrixUpdated(false),
m_viewportUpdated(false),
m_aspectRatio(0.f),
m_fov(70.f),
m_zFar(100.f),
m_zNear(1.f)
{
}

NzCamera::~NzCamera()
{
	if (m_target)
		m_target->RemoveListener(this);
}

void NzCamera::Activate()
{
	#ifdef NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return;
	}
	#endif

	if (!m_viewportUpdated)
		UpdateViewport();

	NzRenderer::SetTarget(m_target);
	NzRenderer::SetViewport(m_viewport);

	if (m_scene)
		m_scene->SetActiveCamera(this);
}

void NzCamera::EnsureFrustumUpdate() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();
}

void NzCamera::EnsureProjectionMatrixUpdate() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();
}

void NzCamera::EnsureViewMatrixUpdate() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();
}

float NzCamera::GetAspectRatio() const
{
	return m_aspectRatio;
}

const NzBoundingVolumef& NzCamera::GetBoundingVolume() const
{
	///TODO: Remplacer par le bounding volume du Frustum ?
	static NzBoundingVolumef dummy(nzExtend_Null);
	return dummy;
}

float NzCamera::GetFOV() const
{
	return m_fov;
}

const NzFrustumf& NzCamera::GetFrustum() const
{
	if (!m_frustumUpdated)
		UpdateFrustum();

	return m_frustum;
}

const NzMatrix4f& NzCamera::GetProjectionMatrix() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	return m_projectionMatrix;
}

nzSceneNodeType NzCamera::GetSceneNodeType() const
{
	return nzSceneNodeType_Camera;
}

const NzRenderTarget* NzCamera::GetTarget() const
{
	return m_target;
}

const NzRectf& NzCamera::GetTargetRegion() const
{
	return m_targetRegion;
}

const NzMatrix4f& NzCamera::GetViewMatrix() const
{
	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	return m_viewMatrix;
}

const NzRectui& NzCamera::GetViewport() const
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return m_viewport;
	}
	#endif

	if (!m_viewportUpdated)
		UpdateViewport();

	return m_viewport;
}

float NzCamera::GetZFar() const
{
	return m_zFar;
}

float NzCamera::GetZNear() const
{
	return m_zNear;
}

void NzCamera::SetFOV(float fov)
{
	m_fov = fov;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzCamera::SetTarget(const NzRenderTarget* renderTarget)
{
	if (m_target)
		m_target->RemoveListener(this);

	m_target = renderTarget;
	if (m_target)
		m_target->AddListener(this);
}

void NzCamera::SetTarget(const NzRenderTarget& renderTarget)
{
	SetTarget(&renderTarget);
}

void NzCamera::SetTargetRegion(const NzRectf& region)
{
	m_targetRegion = region;
	m_viewportUpdated = false;
}

void NzCamera::SetViewport(const NzRectui& viewport)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_target)
	{
		NazaraError("Camera has no render target");
		return;
	}
	#endif

	// On calcule la région nécessaire pour produire ce viewport avec la taille actuelle de la cible
	float invWidth = 1.f/m_target->GetWidth();
	float invHeight = 1.f/m_target->GetHeight();

	SetTargetRegion(NzRectf(invWidth * viewport.x, invHeight * viewport.y, invWidth * viewport.width, invHeight * viewport.height));
}

void NzCamera::SetZFar(float zFar)
{
	m_zFar = zFar;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzCamera::SetZNear(float zNear)
{
	m_zNear = zNear;

	m_frustumUpdated = false;
	m_projectionMatrixUpdated = false;
}

void NzCamera::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	NazaraUnused(renderQueue);

	NazaraInternalError("SceneNode::AddToRenderQueue() called on Camera");
}

void NzCamera::Invalidate()
{
	NzSceneNode::Invalidate();

	// Le frustum et la view matrix dépendent des paramètres du node, invalidons-les
	m_frustumUpdated = false;
	m_viewMatrixUpdated = false;
}

void NzCamera::OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(userdata);

	if (renderTarget == m_target)
		m_target = nullptr;
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));
}

bool NzCamera::OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata)
{
	NazaraUnused(userdata);

	if (renderTarget == m_target)
		m_viewportUpdated = false;
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(renderTarget));

	return true;
}

void NzCamera::Register()
{
}

void NzCamera::Unregister()
{
}

void NzCamera::UpdateFrustum() const
{
	if (!m_projectionMatrixUpdated)
		UpdateProjectionMatrix();

	if (!m_viewMatrixUpdated)
		UpdateViewMatrix();

	m_frustum.Extract(m_viewMatrix, m_projectionMatrix);
	m_frustumUpdated = true;
}

void NzCamera::UpdateProjectionMatrix() const
{
	m_projectionMatrix.MakePerspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
	m_projectionMatrixUpdated = true;
}

void NzCamera::UpdateViewMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_viewMatrix.MakeViewMatrix(m_derivedPosition, m_derivedRotation);
	m_viewMatrixUpdated = true;
}

void NzCamera::UpdateViewport() const
{
	unsigned int width = m_target->GetWidth();
	unsigned int height = std::max(m_target->GetHeight(), 1U);

	float vWidth = width * m_viewport.width;
	float vHeight = height * m_viewport.height;
	float aspectRatio = vWidth/vHeight;

	if (!NzNumberEquals(m_aspectRatio, aspectRatio, 0.001f))
	{
		m_aspectRatio = aspectRatio;
		m_frustumUpdated = false;
		m_projectionMatrixUpdated = false;
	}

	m_viewport.x = width * m_targetRegion.x;
	m_viewport.y = height * m_targetRegion.y;
	m_viewport.width = vWidth;
	m_viewport.height = vHeight;
	m_viewportUpdated = true;
}

bool NzCamera::VisibilityTest(const NzCamera* camera)
{
	NazaraUnused(camera);
	//NazaraInternalError("SceneNode::IsVisible() called on Camera");
	return false;
}
