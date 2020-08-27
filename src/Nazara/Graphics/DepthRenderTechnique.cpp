// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneData.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		struct BillboardPoint
		{
			Color color;
			Vector3f position;
			Vector2f size;
			Vector2f sinCos; // must follow `size` (both will be sent as a Vector4f)
			Vector2f uv;
		};

		constexpr UInt32 s_vertexBufferSize = 4 * 1024 * 1024; // 4 MiB
		constexpr UInt32 s_maxQuadPerDraw = s_vertexBufferSize / sizeof(VertexLayout_XYZ_Color_UV);
	}

	/*!
	* \ingroup graphics
	* \class Nz::DepthRenderTechnique
	* \brief Graphics class that represents the technique used in depth rendering
	*/

	/*!
	* \brief Constructs a DepthRenderTechnique object by default
	*/

	DepthRenderTechnique::DepthRenderTechnique() :
	m_vertexBuffer(BufferType_Vertex)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		m_whiteTexture = Nz::TextureLibrary::Get("White2D");

		m_vertexBuffer.Create(s_vertexBufferSize, DataStorage_Hardware, BufferUsage_Dynamic);

		m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
		m_spriteBuffer.Reset(VertexDeclaration::Get(VertexLayout_XYZ_Color_UV), &m_vertexBuffer);
	}

	/*!
	* \brief Clears the data
	*
	* \param sceneData Data of the scene
	*/

	void DepthRenderTechnique::Clear(const SceneData& sceneData) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		Renderer::SetScissorRect(fullscreenScissorRect);

		Renderer::Enable(RendererParameter_DepthBuffer, true);
		Renderer::Enable(RendererParameter_DepthWrite, true);
		Renderer::Clear(RendererBuffer_Depth);

		// Just in case the background does render depth
		if (sceneData.background)
			sceneData.background->Draw(sceneData.viewer);
	}

	/*!
	* \brief Draws the data of the scene
	* \return true If successful
	*
	* \param sceneData Data of the scene
	*/

	bool DepthRenderTechnique::Draw(const SceneData& sceneData) const
	{
		m_renderQueue.Sort(sceneData.viewer);

		if (!m_renderQueue.models.empty())
			DrawModels(sceneData, m_renderQueue, m_renderQueue.models);

		if (!m_renderQueue.basicSprites.empty())
			DrawSprites(sceneData, m_renderQueue, m_renderQueue.basicSprites);

		if (!m_renderQueue.billboards.empty())
			DrawBillboards(sceneData, m_renderQueue, m_renderQueue.billboards);

		if (!m_renderQueue.depthSortedModels.empty())
			DrawModels(sceneData, m_renderQueue, m_renderQueue.depthSortedModels);

		if (!m_renderQueue.depthSortedSprites.empty())
			DrawSprites(sceneData, m_renderQueue, m_renderQueue.depthSortedSprites);

		if (!m_renderQueue.depthSortedBillboards.empty())
			DrawBillboards(sceneData, m_renderQueue, m_renderQueue.depthSortedBillboards);

		if (!m_renderQueue.customDrawables.empty())
			DrawCustomDrawables(sceneData, m_renderQueue, m_renderQueue.customDrawables);

		return true;
	}

	/*!
	* \brief Gets the render queue
	* \return Pointer to the render queue
	*/

	AbstractRenderQueue* DepthRenderTechnique::GetRenderQueue()
	{
		return &m_renderQueue;
	}

	/*!
	* \brief Gets the type of the current technique
	* \return Type of the render technique
	*/

	RenderTechniqueType DepthRenderTechnique::GetType() const
	{
		return RenderTechniqueType_Depth;
	}

	/*!
	* \brief Initializes the depth render technique
	* \return true If successful
	*
	* \remark Produces a NazaraError if one shader creation failed
	*/

	bool DepthRenderTechnique::Initialize()
	{
		try
		{
			ErrorFlags flags(ErrorFlag_ThrowException, true);

			s_quadIndexBuffer.Reset(true, s_maxQuadPerDraw * 6, DataStorage_Hardware, 0);

			BufferMapper<IndexBuffer> mapper(s_quadIndexBuffer, BufferAccess_WriteOnly);
			UInt32* indices = static_cast<UInt32*>(mapper.GetPointer());

			for (UInt32 i = 0; i < s_maxQuadPerDraw; ++i)
			{
				*indices++ = i * 4 + 0;
				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 1;

				*indices++ = i * 4 + 2;
				*indices++ = i * 4 + 3;
				*indices++ = i * 4 + 1;
			}

			mapper.Unmap(); // Inutile de garder le buffer ouvert plus longtemps

			// Quad buffer (utilisé pour l'instancing de billboard et de sprites)
			//Note: Les UV sont calculés dans le shader
			s_quadVertexBuffer.Reset(VertexDeclaration::Get(VertexLayout_XY), 4, DataStorage_Hardware, 0);

			float vertices[2 * 4] = {
			   -0.5f, -0.5f,
			    0.5f, -0.5f,
			   -0.5f, 0.5f,
			    0.5f, 0.5f,
			};

			s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

			// Déclaration lors du rendu des billboards par sommet
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Color, ComponentType_Color, NazaraOffsetOf(BillboardPoint, color));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Position, ComponentType_Float3, NazaraOffsetOf(BillboardPoint, position));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_TexCoord, ComponentType_Float2, NazaraOffsetOf(BillboardPoint, uv));
			s_billboardVertexDeclaration.EnableComponent(VertexComponent_Userdata0, ComponentType_Float4, NazaraOffsetOf(BillboardPoint, size)); // Englobe sincos

			// Declaration utilisée lors du rendu des billboards par instancing
			// L'avantage ici est la copie directe (std::memcpy) des données de la RenderQueue vers le buffer GPU
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData0, ComponentType_Float3, NazaraOffsetOf(BasicRenderQueue::BillboardData, center));
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData1, ComponentType_Float4, NazaraOffsetOf(BasicRenderQueue::BillboardData, size)); // Englobe sincos
			s_billboardInstanceDeclaration.EnableComponent(VertexComponent_InstanceData2, ComponentType_Color, NazaraOffsetOf(BasicRenderQueue::BillboardData, color));
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialise: " + String(e.what()));
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the depth render technique
	*/

	void DepthRenderTechnique::Uninitialize()
	{
		s_quadIndexBuffer.Reset();
		s_quadVertexBuffer.Reset();
	}
	
	void DepthRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::Billboard>& billboards) const
	{
		VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		Renderer::SetVertexBuffer(&s_quadVertexBuffer);

		Nz::BufferMapper<VertexBuffer> instanceBufferMapper;
		std::size_t billboardCount = 0;
		std::size_t maxBillboardPerDraw = instanceBuffer->GetVertexCount();

		auto Commit = [&]()
		{
			if (billboardCount > 0)
			{
				instanceBufferMapper.Unmap();

				Renderer::DrawPrimitivesInstanced(billboardCount, PrimitiveMode_TriangleStrip, 0, 4);

				billboardCount = 0;
			}
		};

		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		for (const BasicRenderQueue::Billboard& billboard : billboards)
		{
			const Nz::Recti& scissorRect = (billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect;

			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
			{
				Commit();

				const MaterialPipeline* pipeline = billboard.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Deferred | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			if (!instanceBufferMapper.GetBuffer())
				instanceBufferMapper.Map(instanceBuffer, BufferAccess_DiscardAndWrite);

			std::memcpy(static_cast<Nz::UInt8*>(instanceBufferMapper.GetPointer()) + sizeof(BasicRenderQueue::BillboardData) * billboardCount, &billboard.data, sizeof(BasicRenderQueue::BillboardData));
			if (++billboardCount >= maxBillboardPerDraw)
				Commit();
		}

		Commit();
	}
	
	void DepthRenderTechnique::DrawBillboards(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::BillboardChain>& billboards) const
	{
		VertexBuffer* instanceBuffer = Renderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		Renderer::SetVertexBuffer(&s_quadVertexBuffer);

		Nz::BufferMapper<VertexBuffer> instanceBufferMapper;
		std::size_t billboardCount = 0;
		std::size_t maxBillboardPerDraw = instanceBuffer->GetVertexCount();

		auto Commit = [&]()
		{
			if (billboardCount > 0)
			{
				instanceBufferMapper.Unmap();

				Renderer::DrawPrimitivesInstanced(billboardCount, PrimitiveMode_TriangleStrip, 0, 4);

				billboardCount = 0;
			}
		};

		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		for (const BasicRenderQueue::BillboardChain& billboard : billboards)
		{
			const Nz::Recti& scissorRect = (billboard.scissorRect.width > 0) ? billboard.scissorRect : fullscreenScissorRect;

			if (billboard.material != lastMaterial || (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
			{
				Commit();

				const MaterialPipeline* pipeline = billboard.material->GetPipeline();
				if (lastPipeline != pipeline)
				{
					pipelineInstance = &billboard.material->GetPipeline()->Apply(ShaderFlags_Billboard | ShaderFlags_Deferred | ShaderFlags_Instancing | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (lastMaterial != billboard.material)
				{
					billboard.material->Apply(*pipelineInstance);
					lastMaterial = billboard.material;
				}

				if (billboard.material->IsScissorTestEnabled() && scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			std::size_t billboardRemaining = billboard.billboardCount;
			const BasicRenderQueue::BillboardData* billboardData = renderQueue.GetBillboardData(billboard.billboardIndex);
			do
			{
				std::size_t renderedBillboardCount = std::min(billboardRemaining, maxBillboardPerDraw - billboardCount);
				billboardRemaining -= renderedBillboardCount;

				if (!instanceBufferMapper.GetBuffer())
					instanceBufferMapper.Map(instanceBuffer, BufferAccess_DiscardAndWrite);

				std::memcpy(static_cast<Nz::UInt8*>(instanceBufferMapper.GetPointer()) + sizeof(BasicRenderQueue::BillboardData) * billboardCount, billboardData, renderedBillboardCount * sizeof(BasicRenderQueue::BillboardData));
				billboardCount += renderedBillboardCount;
				billboardData += renderedBillboardCount;

				if (billboardCount >= maxBillboardPerDraw)
					Commit();
			}
			while (billboardRemaining > 0);
		}

		Commit();
	}

	void DepthRenderTechnique::DrawCustomDrawables(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::CustomDrawable>& customDrawables) const
	{
		for (const BasicRenderQueue::CustomDrawable& customDrawable : customDrawables)
			customDrawable.drawable->Draw();
	}

	void DepthRenderTechnique::DrawModels(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const Nz::RenderQueue<Nz::BasicRenderQueue::Model>& models) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		///TODO: Reimplement instancing

		for (const BasicRenderQueue::Model& model : models)
		{
			const MaterialPipeline* pipeline = model.material->GetPipeline();
			if (lastPipeline != pipeline)
			{
				pipelineInstance = &model.material->GetPipeline()->Apply(ShaderFlags_Deferred);

				const Shader* shader = pipelineInstance->uberInstance->GetShader();
				if (shader != lastShader)
				{
					// Index of uniforms in the shader
					shaderUniforms = GetShaderUniforms(shader);

					// Ambient color of the scene
					shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);

					lastShader = shader;
				}

				lastPipeline = pipeline;
			}

			if (lastMaterial != model.material)
			{
				model.material->Apply(*pipelineInstance);
				lastMaterial = model.material;
			}

			if (model.material->IsScissorTestEnabled())
			{
				const Nz::Recti& scissorRect = (model.scissorRect.width > 0) ? model.scissorRect : fullscreenScissorRect;
				if (scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(scissorRect);
					lastScissorRect = scissorRect;
				}
			}

			// Handle draw call before rendering loop
			Renderer::DrawCall drawFunc;
			Renderer::DrawCallInstanced instancedDrawFunc;
			unsigned int indexCount;

			if (model.meshData.indexBuffer)
			{
				drawFunc = Renderer::DrawIndexedPrimitives;
				instancedDrawFunc = Renderer::DrawIndexedPrimitivesInstanced;
				indexCount = model.meshData.indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = Renderer::DrawPrimitives;
				instancedDrawFunc = Renderer::DrawPrimitivesInstanced;
				indexCount = model.meshData.vertexBuffer->GetVertexCount();
			}

			Renderer::SetIndexBuffer(model.meshData.indexBuffer);
			Renderer::SetVertexBuffer(model.meshData.vertexBuffer);

			Renderer::SetMatrix(MatrixType_World, model.matrix);
			drawFunc(model.meshData.primitiveMode, 0, indexCount);
		}
	}

	void DepthRenderTechnique::DrawSprites(const SceneData& sceneData, const BasicRenderQueue& renderQueue, const RenderQueue<BasicRenderQueue::SpriteChain>& spriteList) const
	{
		const RenderTarget* renderTarget = sceneData.viewer->GetTarget();
		Recti fullscreenScissorRect = Recti(Vector2i(renderTarget->GetSize()));

		const std::size_t maxSpriteCount = std::min<std::size_t>(s_maxQuadPerDraw, m_spriteBuffer.GetVertexCount() / 4);

		const unsigned int overlayTextureUnit = Material::GetTextureUnit(TextureMap_Overlay);
		const Material* lastMaterial = nullptr;
		const MaterialPipeline* lastPipeline = nullptr;
		const Shader* lastShader = nullptr;
		const ShaderUniforms* shaderUniforms = nullptr;
		const Texture* lastOverlay = nullptr;
		Recti lastScissorRect = Recti(-1, -1);

		const MaterialPipeline::Instance* pipelineInstance = nullptr;

		Renderer::SetIndexBuffer(&s_quadIndexBuffer);
		Renderer::SetMatrix(MatrixType_World, Matrix4f::Identity());
		Renderer::SetVertexBuffer(&m_spriteBuffer);

		auto Draw = [&]()
		{
			unsigned int firstIndex = 0;
			for (const auto& batch : m_spriteBatches)
			{
				const MaterialPipeline* pipeline = batch.material->GetPipeline();
				if (pipeline != lastPipeline)
				{
					pipelineInstance = &batch.material->GetPipeline()->Apply(ShaderFlags_TextureOverlay | ShaderFlags_VertexColor);

					const Shader* shader = pipelineInstance->uberInstance->GetShader();
					if (shader != lastShader)
					{
						// Index of uniforms in the shader
						shaderUniforms = GetShaderUniforms(shader);

						// Ambient color of the scene
						shader->SendColor(shaderUniforms->sceneAmbient, sceneData.ambientColor);

						// Overlay texture unit
						shader->SendInteger(shaderUniforms->textureOverlay, overlayTextureUnit);

						lastShader = shader;
					}

					lastPipeline = pipeline;
				}

				if (batch.material != lastMaterial)
				{
					batch.material->Apply(*pipelineInstance);

					Renderer::SetTextureSampler(overlayTextureUnit, batch.material->GetDiffuseSampler());

					lastMaterial = batch.material;
				}

				if (batch.overlayTexture != lastOverlay)
				{
					Renderer::SetTexture(overlayTextureUnit, batch.overlayTexture);
					lastOverlay = batch.overlayTexture;
				}

				if (batch.material->IsScissorTestEnabled() && batch.scissorRect != lastScissorRect)
				{
					Renderer::SetScissorRect(batch.scissorRect);
					lastScissorRect = batch.scissorRect;
				}

				unsigned int indexCount = batch.spriteCount * 6;
				Renderer::DrawIndexedPrimitives(PrimitiveMode_TriangleList, firstIndex, indexCount);
				firstIndex += indexCount;
			}
		};

		m_spriteBatches.clear();
		{
			BufferMapper<VertexBuffer> vertexMapper;
			VertexStruct_XYZ_Color_UV* vertices = nullptr;

			std::size_t remainingSprite = maxSpriteCount;

			const Material* lastMaterial = nullptr;
			const Texture* lastOverlay = nullptr;
			Recti lastScissorRect = Recti(-1, -1);

			for (const BasicRenderQueue::SpriteChain& basicSprites : spriteList)
			{
				const Nz::Texture* overlayTexture = (basicSprites.overlay) ? basicSprites.overlay.Get() : m_whiteTexture.Get();
				const Nz::Recti& scissorRect = (basicSprites.scissorRect.width > 0) ? basicSprites.scissorRect : fullscreenScissorRect;

				const VertexStruct_XYZ_Color_UV* spriteVertices = basicSprites.vertices;
				std::size_t spriteCount = basicSprites.spriteCount;

				for (;;)
				{
					if (m_spriteBatches.empty() || basicSprites.material != lastMaterial || overlayTexture != lastOverlay || (basicSprites.material->IsScissorTestEnabled() && scissorRect != lastScissorRect))
					{
						m_spriteBatches.emplace_back();
						SpriteBatch& newBatch = m_spriteBatches.back();
						newBatch.material = basicSprites.material;
						newBatch.overlayTexture = overlayTexture;
						newBatch.scissorRect = scissorRect;
						newBatch.spriteCount = 0;

						lastMaterial = basicSprites.material;
						lastOverlay = overlayTexture;
						lastScissorRect = scissorRect;
					}

					SpriteBatch& currentBatch = m_spriteBatches.back();

					if (!vertices)
					{
						vertexMapper.Map(m_spriteBuffer, BufferAccess_DiscardAndWrite);
						vertices = static_cast<VertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());
					}

					std::size_t processedSpriteCount = std::min(remainingSprite, spriteCount);
					std::size_t processedVertices = processedSpriteCount * 4;

					std::memcpy(vertices, spriteVertices, processedVertices * sizeof(VertexStruct_XYZ_Color_UV));
					vertices += processedVertices;
					spriteVertices += processedVertices;

					currentBatch.spriteCount += processedSpriteCount;
					spriteCount -= processedSpriteCount;

					remainingSprite -= processedSpriteCount;
					if (remainingSprite == 0)
					{
						vertexMapper.Unmap();
						vertices = nullptr;

						Draw();

						remainingSprite = maxSpriteCount;
						m_spriteBatches.clear();
					}

					if (spriteCount == 0)
						break;
				}
			}
		}

		Draw();
	}

	/*!
	* \brief Gets the shader uniforms
	* \return Uniforms of the shader
	*
	* \param shader Shader to get uniforms from
	*/

	const DepthRenderTechnique::ShaderUniforms* DepthRenderTechnique::GetShaderUniforms(const Shader* shader) const
	{
		auto it = m_shaderUniforms.find(shader);
		if (it == m_shaderUniforms.end())
		{
			ShaderUniforms uniforms;
			uniforms.shaderReleaseSlot.Connect(shader->OnShaderRelease, this, &DepthRenderTechnique::OnShaderInvalidated);
			uniforms.shaderUniformInvalidatedSlot.Connect(shader->OnShaderUniformInvalidated, this, &DepthRenderTechnique::OnShaderInvalidated);

			uniforms.sceneAmbient   = shader->GetUniformLocation("SceneAmbient");
			uniforms.textureOverlay = shader->GetUniformLocation("TextureOverlay");

			it = m_shaderUniforms.emplace(shader, std::move(uniforms)).first;
		}

		return &it->second;
	}

	/*!
	* \brief Handle the invalidation of a shader
	*
	* \param shader Shader being invalidated
	*/

	void DepthRenderTechnique::OnShaderInvalidated(const Shader* shader) const
	{
		m_shaderUniforms.erase(shader);
	}

	IndexBuffer DepthRenderTechnique::s_quadIndexBuffer;
	VertexBuffer DepthRenderTechnique::s_quadVertexBuffer;
	VertexDeclaration DepthRenderTechnique::s_billboardInstanceDeclaration;
	VertexDeclaration DepthRenderTechnique::s_billboardVertexDeclaration;
}
