// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/RenderSystem.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>

namespace Ndk
{
	RenderSystem::RenderSystem() :
	m_coordinateSystemMatrix(Nz::Matrix4f::Identity()),
	m_coordinateSystemInvalidated(true)
	{
		ChangeRenderTechnique<Nz::ForwardRenderTechnique>();
		SetDefaultBackground(Nz::ColorBackground::New());
		SetUpdateRate(0.f);
	}

	void RenderSystem::OnEntityRemoved(Entity* entity)
	{
		m_cameras.Remove(entity);
		m_directionalLights.Remove(entity);
		m_drawables.Remove(entity);
		m_lights.Remove(entity);
		m_particleGroups.Remove(entity);
		m_pointSpotLights.Remove(entity);
	}

	void RenderSystem::OnEntityValidation(Entity* entity, bool justAdded)
	{
		NazaraUnused(justAdded);

		if (entity->HasComponent<CameraComponent>() && entity->HasComponent<NodeComponent>())
		{
			m_cameras.Insert(entity);
			std::sort(m_cameras.begin(), m_cameras.end(), [](const EntityHandle& handle1, const EntityHandle& handle2)
			{
				return handle1->GetComponent<CameraComponent>().GetLayer() < handle2->GetComponent<CameraComponent>().GetLayer();
			});
		}
		else
			m_cameras.Remove(entity);

		if (entity->HasComponent<GraphicsComponent>() && entity->HasComponent<NodeComponent>())
			m_drawables.Insert(entity);
		else
			m_drawables.Remove(entity);

		if (entity->HasComponent<LightComponent>() && entity->HasComponent<NodeComponent>())
		{
			LightComponent& lightComponent = entity->GetComponent<LightComponent>();
			if (lightComponent.GetLightType() == Nz::LightType_Directional)
			{
				m_directionalLights.Insert(entity);
				m_pointSpotLights.Remove(entity);
			}
			else
			{
				m_directionalLights.Remove(entity);
				m_pointSpotLights.Insert(entity);
			}

			m_lights.Insert(entity);
		}
		else
		{
			m_directionalLights.Remove(entity);
			m_lights.Remove(entity);
			m_pointSpotLights.Remove(entity);
		}

		if (entity->HasComponent<ParticleGroupComponent>())
			m_particleGroups.Insert(entity);
		else
			m_particleGroups.Remove(entity);
	}

	void RenderSystem::OnUpdate(float elapsedTime)
	{
		NazaraUnused(elapsedTime);

		// Invalidate every renderable if the coordinate system changed
		if (m_coordinateSystemInvalidated)
		{
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				graphicsComponent.InvalidateTransformMatrix();
			}

			m_coordinateSystemInvalidated = false;
		}

		UpdatePointSpotShadowMaps();

		for (const Ndk::EntityHandle& camera : m_cameras)
		{
			CameraComponent& camComponent = camera->GetComponent<CameraComponent>();

			//UpdateDirectionalShadowMaps(camComponent);

			Nz::AbstractRenderQueue* renderQueue = m_renderTechnique->GetRenderQueue();
			renderQueue->Clear();

			//TODO: Culling
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

				graphicsComponent.AddToRenderQueue(renderQueue);
			}

			for (const Ndk::EntityHandle& light : m_lights)
			{
				LightComponent& lightComponent = light->GetComponent<LightComponent>();
				NodeComponent& lightNode = light->GetComponent<NodeComponent>();

				///TODO: Cache somehow?
				lightComponent.AddToRenderQueue(renderQueue, Nz::Matrix4f::ConcatenateAffine(m_coordinateSystemMatrix, lightNode.GetTransformMatrix()));
			}

			for (const Ndk::EntityHandle& particleGroup : m_particleGroups)
			{
				ParticleGroupComponent& groupComponent = particleGroup->GetComponent<ParticleGroupComponent>();

				groupComponent.AddToRenderQueue(renderQueue, Nz::Matrix4f::Identity()); //< ParticleGroup doesn't use Matrix4f
			}

			camComponent.ApplyView();

			Nz::SceneData sceneData;
			sceneData.ambientColor = Nz::Color(25, 25, 25);
			sceneData.background = m_background;
			sceneData.viewer = &camComponent;

			m_renderTechnique->Clear(sceneData);
			m_renderTechnique->Draw(sceneData);
		}
	}

	void RenderSystem::UpdateDirectionalShadowMaps(const Nz::AbstractViewer& viewer)
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		Nz::SceneData dummySceneData;
		dummySceneData.ambientColor = Nz::Color(0, 0, 0);
		dummySceneData.background = nullptr;
		dummySceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

		for (const Ndk::EntityHandle& light : m_directionalLights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled())
				continue;

			Nz::Vector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap());
			Nz::Renderer::SetTarget(&m_shadowRT);
			Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

			Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
			renderQueue->Clear();

			///TODO: Culling
			for (const Ndk::EntityHandle& drawable : m_drawables)
			{
				GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
				NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

				graphicsComponent.AddToRenderQueue(renderQueue);
			}

			///TODO: Cache the matrices in the light?
			Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Ortho(0.f, 100.f, 100.f, 0.f, 1.f, 100.f));
			Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetRotation() * Nz::Vector3f::Forward() * 100.f, lightNode.GetRotation()));

			m_shadowTechnique.Clear(dummySceneData);
			m_shadowTechnique.Draw(dummySceneData);
		}
	}

	void RenderSystem::UpdatePointSpotShadowMaps()
	{
		if (!m_shadowRT.IsValid())
			m_shadowRT.Create();

		Nz::SceneData dummySceneData;
		dummySceneData.ambientColor = Nz::Color(0, 0, 0);
		dummySceneData.background = nullptr;
		dummySceneData.viewer = nullptr; //< Depth technique doesn't require any viewer

		for (const Ndk::EntityHandle& light : m_pointSpotLights)
		{
			LightComponent& lightComponent = light->GetComponent<LightComponent>();
			NodeComponent& lightNode = light->GetComponent<NodeComponent>();

			if (!lightComponent.IsShadowCastingEnabled())
				continue;

			Nz::Vector2ui shadowMapSize(lightComponent.GetShadowMap()->GetSize());

			switch (lightComponent.GetLightType())
			{
				case Nz::LightType_Directional:
					NazaraInternalError("Directional lights included in point/spot light list");
					break;

				case Nz::LightType_Point:
				{
					static Nz::Quaternionf rotations[6] =
					{
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitX()), // nzCubemapFace_PositiveX
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitX()), // nzCubemapFace_NegativeX
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitY()), // nzCubemapFace_PositiveY
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitY()), // nzCubemapFace_NegativeY
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -Nz::Vector3f::UnitZ()), // nzCubemapFace_PositiveZ
						Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(),  Nz::Vector3f::UnitZ())  // nzCubemapFace_NegativeZ
					};

					for (unsigned int face = 0; face < 6; ++face)
					{
						m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap(), face);
						Nz::Renderer::SetTarget(&m_shadowRT);
						Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

						///TODO: Cache the matrices in the light?
						Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Perspective(Nz::FromDegrees(90.f), 1.f, 0.1f, lightComponent.GetRadius()));
						Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetPosition(), rotations[face]));

						Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
						renderQueue->Clear();

						///TODO: Culling
						for (const Ndk::EntityHandle& drawable : m_drawables)
						{
							GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
							NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

							graphicsComponent.AddToRenderQueue(renderQueue);
						}

						m_shadowTechnique.Clear(dummySceneData);
						m_shadowTechnique.Draw(dummySceneData);
					}
					break;
				}

				case Nz::LightType_Spot:
				{
					m_shadowRT.AttachTexture(Nz::AttachmentPoint_Depth, 0, lightComponent.GetShadowMap());
					Nz::Renderer::SetTarget(&m_shadowRT);
					Nz::Renderer::SetViewport(Nz::Recti(0, 0, shadowMapSize.x, shadowMapSize.y));

					///TODO: Cache the matrices in the light?
					Nz::Renderer::SetMatrix(Nz::MatrixType_Projection, Nz::Matrix4f::Perspective(lightComponent.GetOuterAngle()*2.f, 1.f, 0.1f, lightComponent.GetRadius()));
					Nz::Renderer::SetMatrix(Nz::MatrixType_View, Nz::Matrix4f::ViewMatrix(lightNode.GetPosition(), lightNode.GetRotation()));

					Nz::AbstractRenderQueue* renderQueue = m_shadowTechnique.GetRenderQueue();
					renderQueue->Clear();

					///TODO: Culling
					for (const Ndk::EntityHandle& drawable : m_drawables)
					{
						GraphicsComponent& graphicsComponent = drawable->GetComponent<GraphicsComponent>();
						NodeComponent& drawableNode = drawable->GetComponent<NodeComponent>();

						graphicsComponent.AddToRenderQueue(renderQueue);
					}

					m_shadowTechnique.Clear(dummySceneData);
					m_shadowTechnique.Draw(dummySceneData);
					break;
				}
			}
		}
	}

	SystemIndex RenderSystem::systemIndex;
}
