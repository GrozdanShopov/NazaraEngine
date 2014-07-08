// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Surveiller les shaders et supprimer les données uniformes en cas de changement (recompilation/destruction)

namespace
{
	static NzIndexBuffer* s_indexBuffer = nullptr;
	unsigned int s_maxSprites = 8192;

	NzIndexBuffer* BuildIndexBuffer()
	{
		std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(false, s_maxSprites*6, nzBufferStorage_Hardware, nzBufferUsage_Static));
		indexBuffer->SetPersistent(false);

		NzBufferMapper<NzIndexBuffer> mapper(indexBuffer.get(), nzBufferAccess_WriteOnly);
		nzUInt16* indices = static_cast<nzUInt16*>(mapper.GetPointer());

		for (unsigned int i = 0; i < s_maxSprites; ++i)
		{
			*indices++ = i*4 + 0;
			*indices++ = i*4 + 2;
			*indices++ = i*4 + 1;

			*indices++ = i*4 + 2;
			*indices++ = i*4 + 3;
			*indices++ = i*4 + 1;
		}

		return indexBuffer.release();
	}
}

NzForwardRenderTechnique::NzForwardRenderTechnique() :
m_spriteBuffer(NzVertexDeclaration::Get(nzVertexLayout_XYZ_UV), s_maxSprites*4, nzBufferStorage_Hardware, nzBufferUsage_Dynamic),
m_maxLightPassPerObject(3)
{
	if (!s_indexBuffer)
		s_indexBuffer = BuildIndexBuffer();

	m_indexBuffer = s_indexBuffer;
}

NzForwardRenderTechnique::~NzForwardRenderTechnique()
{
	if (m_indexBuffer.Reset())
		s_indexBuffer = nullptr;
}

void NzForwardRenderTechnique::Clear(const NzScene* scene) const
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererBuffer_Depth);

	NzAbstractBackground* background = (scene->IsBackgroundEnabled()) ? scene->GetBackground() : nullptr;
	if (background)
		background->Draw(scene);
}

bool NzForwardRenderTechnique::Draw(const NzScene* scene) const
{
	m_directionalLights.SetLights(&m_renderQueue.directionalLights[0], m_renderQueue.directionalLights.size());
	m_lights.SetLights(&m_renderQueue.lights[0], m_renderQueue.lights.size());
	m_renderQueue.Sort(scene->GetViewer());

	if (!m_renderQueue.opaqueModels.empty())
		DrawOpaqueModels(scene);

	if (!m_renderQueue.transparentModels.empty())
		DrawTransparentModels(scene);

	if (!m_renderQueue.sprites.empty())
		DrawSprites(scene);

	// Les autres drawables (Exemple: Terrain)
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();

	return true;

	// Les billboards
	/*if (!m_renderQueue.billboards.empty())
	{
		//NzRenderer::SetIndexBuffer(m_billboardIndexBuffer);
		NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
		NzRenderer::SetShader(m_billboardShader);
		NzRenderer::SetVertexBuffer(m_billboardVertexBuffer);

		m_billboardShader->SendVector(s_cameraForwardLocation, camera->GetForward());
		m_billboardShader->SendVector(s_cameraUpLocation, camera->GetUp());
		m_billboardShader->SendVector(s_worldUpLocation, NzVector3f::Up());

		// Couleur ambiante de la scène
		m_billboardShader->SendColor(m_billboardShader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
		// Position de la caméra
		m_billboardShader->SendVector(m_billboardShader->GetUniformLocation(nzShaderUniform_CameraPosition), camera->GetPosition());

		lightCount = 0;

		// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
		m_renderQueue.lights[0]->Apply(m_billboardShader, 0);

		for (auto& matIt : m_renderQueue.billboards)
		{
			const NzMaterial* material = matIt.first;
			auto& billboards = matIt.second;

			material->Apply(m_billboardShader);

			unsigned int billboardCount = billboards.size();
			const NzForwardRenderQueue::BillboardData* data = &billboards[0];
			while (billboardCount > 0)
			{
				unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboards);
				billboardCount -= renderedBillboardCount;

				m_billboardVertexBuffer->FillVertices(data, 0, renderedBillboardCount, true);
				data += renderedBillboardCount;

				NzRenderer::DrawPrimitives(nzPrimitiveMode_PointList, 0, renderedBillboardCount);
			}
			billboards.clear();
		}
	}*/
}

unsigned int NzForwardRenderTechnique::GetMaxLightPassPerObject() const
{
	return m_maxLightPassPerObject;
}

NzAbstractRenderQueue* NzForwardRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzForwardRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_BasicForward;
}

void NzForwardRenderTechnique::SetMaxLightPassPerObject(unsigned int passCount)
{
	m_maxLightPassPerObject = passCount;
}

void NzForwardRenderTechnique::DrawOpaqueModels(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const LightUniforms* lightUniforms = nullptr;
	const NzShader* lastShader = nullptr;

	for (auto& matIt : m_renderQueue.opaqueModels)
	{
		bool& used = std::get<0>(matIt.second);
		if (used)
		{
			bool& renderQueueInstancing = std::get<1>(matIt.second);
			NzForwardRenderQueue::MeshInstanceContainer& meshInstances = std::get<2>(matIt.second);

			if (!meshInstances.empty())
			{
				const NzMaterial* material = matIt.first;

				// Nous utilisons de l'instancing que lorsqu'aucune lumière (autre que directionnelle) n'est active
				// Ceci car l'instancing n'est pas compatible avec la recherche des lumières les plus proches
				// (Le deferred shading n'a pas ce problème)
				bool instancing = m_instancingEnabled && (!material->IsLightingEnabled() || m_lights.IsEmpty()) && renderQueueInstancing;

				// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
				const NzShader* shader = material->Apply((instancing) ? nzShaderFlags_Instancing : 0);

				// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
				if (shader != lastShader)
				{
					// Couleur ambiante de la scène
					shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
					// Position de la caméra
					shader->SendVector(shader->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

					// Index des uniformes d'éclairage dans le shader
					lightUniforms = GetLightUniforms(shader);

					lastShader = shader;
				}

				// Meshes
				for (auto& subMeshIt : meshInstances)
				{
					const NzMeshData& meshData = subMeshIt.first;
					const NzSpheref& boundingSphere = subMeshIt.second.first;
					std::vector<NzMatrix4f>& instances = subMeshIt.second.second;

					if (!instances.empty())
					{
						const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
						const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

						// Gestion du draw call avant la boucle de rendu
						std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> DrawFunc;
						std::function<void(unsigned int, nzPrimitiveMode, unsigned int, unsigned int)> InstancedDrawFunc;
						unsigned int indexCount;

						if (indexBuffer)
						{
							DrawFunc = NzRenderer::DrawIndexedPrimitives;
							InstancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
							indexCount = indexBuffer->GetIndexCount();
						}
						else
						{
							DrawFunc = NzRenderer::DrawPrimitives;
							InstancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
							indexCount = vertexBuffer->GetVertexCount();
						}

						NzRenderer::SetIndexBuffer(indexBuffer);
						NzRenderer::SetVertexBuffer(vertexBuffer);

						if (instancing)
						{
							// On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)
							NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();
							instanceBuffer->SetVertexDeclaration(NzVertexDeclaration::Get(nzVertexLayout_Matrix4));

							// Avec l'instancing, impossible de sélectionner les lumières pour chaque objet
							// Du coup, il n'est activé que pour les lumières directionnelles
							unsigned int lightCount = m_directionalLights.GetLightCount();
							unsigned int lightIndex = 0;
							nzRendererComparison oldDepthFunc = NzRenderer::GetDepthFunc();

							unsigned int passCount = (lightCount == 0) ? 1 : (lightCount-1)/NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
							for (unsigned int pass = 0; pass < passCount; ++pass)
							{
								if (lightUniforms->exists)
								{
									unsigned int renderedLightCount = std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
									lightCount -= renderedLightCount;

									if (pass == 1)
									{
										// Pour additionner le résultat des calculs de lumière
										// Aucune chance d'interférer avec les paramètres du matériau car nous ne rendons que les objets opaques
										// (Autrement dit, sans blending)
										// Quant à la fonction de profondeur, elle ne doit être appliquée que la première fois
										NzRenderer::Enable(nzRendererParameter_Blend, true);
										NzRenderer::SetBlendFunc(nzBlendFunc_One, nzBlendFunc_One);
										NzRenderer::SetDepthFunc(nzRendererComparison_Equal);
									}

									for (unsigned int i = 0; i < renderedLightCount; ++i)
										m_directionalLights.GetLight(lightIndex++)->Enable(shader, lightUniforms->uniforms, lightUniforms->offset*i);

									for (unsigned int i = renderedLightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
										NzLight::Disable(shader, lightUniforms->uniforms, lightUniforms->offset*i);
								}

								const NzMatrix4f* instanceMatrices = &instances[0];
								unsigned int instanceCount = instances.size();
								unsigned int maxInstanceCount = instanceBuffer->GetVertexCount(); // On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)

								while (instanceCount > 0)
								{
									// On calcule le nombre d'instances que l'on pourra afficher cette fois-ci (Selon la taille du buffer d'instancing)
									unsigned int renderedInstanceCount = std::min(instanceCount, maxInstanceCount);
									instanceCount -= renderedInstanceCount;

									// On remplit l'instancing buffer avec nos matrices world
									instanceBuffer->Fill(instanceMatrices, 0, renderedInstanceCount, true);
									instanceMatrices += renderedInstanceCount;

									// Et on affiche
									InstancedDrawFunc(renderedInstanceCount, meshData.primitiveMode, 0, indexCount);
								}
							}

							// On n'oublie pas de désactiver le blending pour ne pas interférer sur le reste du rendu
							NzRenderer::Enable(nzRendererParameter_Blend, false);
							NzRenderer::SetDepthFunc(oldDepthFunc);
						}
						else
						{
							if (lightUniforms->exists)
							{
								for (const NzMatrix4f& matrix : instances)
								{
									unsigned int directionalLightCount = m_directionalLights.GetLightCount();
									unsigned int otherLightCount = m_lights.ComputeClosestLights(matrix.GetTranslation() + boundingSphere.GetPosition(), boundingSphere.radius, m_maxLightPassPerObject*NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS - directionalLightCount);
									unsigned int lightCount = directionalLightCount + otherLightCount;

									NzRenderer::SetMatrix(nzMatrixType_World, matrix);
									unsigned int directionalLightIndex = 0;
									unsigned int otherLightIndex = 0;
									nzRendererComparison oldDepthFunc = NzRenderer::GetDepthFunc(); // Dans le cas où nous aurions à le changer

									unsigned int passCount = (lightCount == 0) ? 1 : (lightCount-1)/NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS + 1;
									for (unsigned int pass = 0; pass < passCount; ++pass)
									{
										unsigned int renderedLightCount = std::min(lightCount, NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
										lightCount -= renderedLightCount;

										if (pass == 1)
										{
											// Pour additionner le résultat des calculs de lumière
											// Aucune chance d'interférer avec les paramètres du matériau car nous ne rendons que les objets opaques
											// (Autrement dit, sans blending)
											// Quant à la fonction de profondeur, elle ne doit être appliquée que la première fois
											NzRenderer::Enable(nzRendererParameter_Blend, true);
											NzRenderer::SetBlendFunc(nzBlendFunc_One, nzBlendFunc_One);
											NzRenderer::SetDepthFunc(nzRendererComparison_Equal);
										}

										// On active les lumières de cette passe-ci
										for (unsigned int i = 0; i < renderedLightCount; ++i)
										{
											if (directionalLightIndex >= directionalLightCount)
												m_lights.GetResult(otherLightIndex++)->Enable(shader, lightUniforms->uniforms, lightUniforms->offset*i);
											else
												m_directionalLights.GetLight(directionalLightIndex++)->Enable(shader, lightUniforms->uniforms, lightUniforms->offset*i);
										}

										// On désactive l'éventuel surplus
										for (unsigned int i = renderedLightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
											NzLight::Disable(shader, lightUniforms->uniforms, lightUniforms->offset*i);

										// Et on passe à l'affichage
										DrawFunc(meshData.primitiveMode, 0, indexCount);
									}

									NzRenderer::Enable(nzRendererParameter_Blend, false);
									NzRenderer::SetDepthFunc(oldDepthFunc);
								}
							}
							else
							{
								// Sans instancing, on doit effectuer un drawcall pour chaque instance
								// Cela reste néanmoins plus rapide que l'instancing en dessous d'un certain nombre d'instances
								// À cause du temps de modification du buffer d'instancing
								for (const NzMatrix4f& matrix : instances)
								{
									NzRenderer::SetMatrix(nzMatrixType_World, matrix);
									DrawFunc(meshData.primitiveMode, 0, indexCount);
								}
							}
						}
						instances.clear();
					}
				}
			}

			// Et on remet à zéro les données
			renderQueueInstancing = false;
			used = false;
		}
	}
}

void NzForwardRenderTechnique::DrawSprites(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const NzShader* lastShader = nullptr;

	NzRenderer::SetIndexBuffer(m_indexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
	NzRenderer::SetVertexBuffer(&m_spriteBuffer);

	for (auto& matIt : m_renderQueue.sprites)
	{
		const NzMaterial* material = matIt.first;
		auto& spriteVector = matIt.second;

		unsigned int spriteCount = spriteVector.size();
		if (spriteCount > 0)
		{
			// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
			const NzShader* shader = material->Apply();

			// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
			if (shader != lastShader)
			{
				// Couleur ambiante de la scène
				shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
				// Position de la caméra
				shader->SendVector(shader->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

				lastShader = shader;
			}

			const NzSprite** spritePtr = &spriteVector[0];
			do
			{
				unsigned int renderedSpriteCount = std::min(spriteCount, 64U);
				spriteCount -= renderedSpriteCount;

				NzBufferMapper<NzVertexBuffer> vertexMapper(m_spriteBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedSpriteCount*4);
				NzVertexStruct_XYZ_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_UV*>(vertexMapper.GetPointer());

				for (unsigned int i = 0; i < renderedSpriteCount; ++i)
				{
					const NzSprite* sprite = *spritePtr++;
					const NzRectf& textureCoords = sprite->GetTextureCoords();
					const NzVector2f& halfSize = sprite->GetSize()*0.5f;
					NzVector3f center = sprite->GetPosition();
					NzQuaternionf rotation = sprite->GetRotation();

					vertices->position = center + rotation * NzVector3f(-halfSize.x, -halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.x, textureCoords.y + textureCoords.height);
					vertices++;

					vertices->position = center + rotation * NzVector3f(halfSize.x, -halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.width, textureCoords.y + textureCoords.height);
					vertices++;

					vertices->position = center + rotation * NzVector3f(-halfSize.x, halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.x, textureCoords.y);
					vertices++;

					vertices->position = center + rotation * NzVector3f(halfSize.x, halfSize.y, 0.f);
					vertices->uv.Set(textureCoords.width, textureCoords.y);
					vertices++;
				}

				vertexMapper.Unmap();

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, renderedSpriteCount*6);
			}
			while (spriteCount > 0);

			spriteVector.clear();
		}
	}
}

void NzForwardRenderTechnique::DrawTransparentModels(const NzScene* scene) const
{
	NzAbstractViewer* viewer = scene->GetViewer();
	const LightUniforms* lightUniforms = nullptr;
	const NzShader* lastShader = nullptr;
	unsigned int lightCount = 0;

	for (unsigned int index : m_renderQueue.transparentModels)
	{
		const NzForwardRenderQueue::TransparentModelData& modelData = m_renderQueue.transparentModelData[index];

		// Matériau
		const NzMaterial* material = modelData.material;

		// On commence par appliquer du matériau (et récupérer le shader ainsi activé)
		const NzShader* shader = material->Apply();

		// Les uniformes sont conservées au sein d'un programme, inutile de les renvoyer tant qu'il ne change pas
		if (shader != lastShader)
		{
			// Couleur ambiante de la scène
			shader->SendColor(shader->GetUniformLocation(nzShaderUniform_SceneAmbient), scene->GetAmbientColor());
			// Position de la caméra
			shader->SendVector(shader->GetUniformLocation(nzShaderUniform_EyePosition), viewer->GetEyePosition());

			// Index des uniformes d'éclairage dans le shader
			lightUniforms = GetLightUniforms(shader);

			// On envoie les lumières directionnelles s'il y a (Les mêmes pour tous)
			lightCount = std::min(m_directionalLights.GetLightCount(), NazaraSuffixMacro(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS, U));
			for (unsigned int i = 0; i < lightCount; ++i)
				m_directionalLights.GetLight(i)->Enable(shader, lightUniforms->uniforms, lightUniforms->offset*i);

			lastShader = shader;
		}

		// Mesh
		const NzMatrix4f& matrix = modelData.transformMatrix;
		const NzMeshData& meshData = modelData.meshData;

		const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
		const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

		// Gestion du draw call avant la boucle de rendu
		std::function<void(nzPrimitiveMode, unsigned int, unsigned int)> DrawFunc;
		unsigned int indexCount;

		if (indexBuffer)
		{
			DrawFunc = NzRenderer::DrawIndexedPrimitives;
			indexCount = indexBuffer->GetIndexCount();
		}
		else
		{
			DrawFunc = NzRenderer::DrawPrimitives;
			indexCount = vertexBuffer->GetVertexCount();
		}

		NzRenderer::SetIndexBuffer(indexBuffer);
		NzRenderer::SetVertexBuffer(vertexBuffer);

		// Calcul des lumières les plus proches
		if (lightCount < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS && !m_lights.IsEmpty())
		{
			unsigned int count = std::min(NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS - lightCount, m_lights.ComputeClosestLights(matrix.GetTranslation() + modelData.boundingSphere.GetPosition(), modelData.boundingSphere.radius, NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS));
			for (unsigned int i = 0; i < count; ++i)
				m_lights.GetResult(i)->Enable(shader, lightUniforms->uniforms, lightUniforms->offset*(lightCount++));
		}

		for (unsigned int i = lightCount; i < NAZARA_GRAPHICS_MAX_LIGHT_PER_PASS; ++i)
			NzLight::Disable(shader, lightUniforms->uniforms, lightUniforms->offset*i);

		NzRenderer::SetMatrix(nzMatrixType_World, matrix);
		DrawFunc(meshData.primitiveMode, 0, indexCount);
	}
}

const NzForwardRenderTechnique::LightUniforms* NzForwardRenderTechnique::GetLightUniforms(const NzShader* shader) const
{
	auto it = m_lightUniforms.find(shader);
	if (it != m_lightUniforms.end())
		return &(it->second);
	else
	{
		int type0Location = shader->GetUniformLocation("Lights[0].type");
		int type1Location = shader->GetUniformLocation("Lights[1].type");

		LightUniforms lightUniforms;

		if (type0Location > 0 && type1Location > 0)
		{
			lightUniforms.exists = true;
			lightUniforms.offset = type1Location - type0Location;
			lightUniforms.uniforms.ubo = false;
			lightUniforms.uniforms.locations.type = type0Location;
			lightUniforms.uniforms.locations.color = shader->GetUniformLocation("Lights[0].color");
			lightUniforms.uniforms.locations.factors = shader->GetUniformLocation("Lights[0].factors");
			lightUniforms.uniforms.locations.parameters1 = shader->GetUniformLocation("Lights[0].parameters1");
			lightUniforms.uniforms.locations.parameters2 = shader->GetUniformLocation("Lights[0].parameters2");
			lightUniforms.uniforms.locations.parameters3 = shader->GetUniformLocation("Lights[0].parameters3");
		}
		else
			lightUniforms.exists = false;

		auto pair = m_lightUniforms.emplace(shader, lightUniforms);
		return &(pair.first->second);
	}
}
