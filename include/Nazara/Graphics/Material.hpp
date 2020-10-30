// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BASE_MATERIAL_HPP
#define NAZARA_BASE_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/UniformBuffer.hpp>
#include <array>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API Material : public RefCounted, public Resource
	{
		public:
			Material(std::shared_ptr<const MaterialSettings> settings);
			inline ~Material();

			inline void Configure(std::shared_ptr<MaterialPipeline> pipeline);
			inline void Configure(const MaterialPipelineInfo& pipelineInfo);

			inline void EnableAlphaTest(bool alphaTest);
			inline void EnableBlending(bool blending);
			inline void EnableColorWrite(bool colorWrite);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthSorting(bool depthSorting);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableReflectionMapping(bool reflection);
			inline void EnableScissorTest(bool scissorTest);
			inline void EnableShadowCasting(bool castShadows);
			inline void EnableShadowReceive(bool receiveShadows);
			inline void EnableStencilTest(bool stencilTest);
			inline void EnableVertexColor(bool vertexColor);

			inline void EnsurePipelineUpdate() const;
			
			inline RendererComparison GetDepthCompareFunc() const;
			inline BlendFunc GetDstBlend() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline float GetLineWidth() const;
			inline const std::shared_ptr<MaterialPipeline>& GetPipeline() const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline const std::shared_ptr<const MaterialSettings>& GetSettings() const;
			inline const std::shared_ptr<ShaderStage>& GetShader(ShaderStageType shaderStage) const;
			inline BlendFunc GetSrcBlend() const;
			inline const std::shared_ptr<Texture>& GetTexture(std::size_t textureIndex) const;
			inline const std::shared_ptr<TextureSampler>& GetTextureSampler(std::size_t textureIndex) const;
			inline UniformBufferRef& GetUniformBuffer(std::size_t bufferIndex);
			inline const UniformBufferRef& GetUniformBuffer(std::size_t bufferIndex) const;

			inline bool HasTexture(std::size_t textureIndex) const;
			inline bool HasVertexColor() const;

			inline bool IsAlphaTestEnabled() const;
			inline bool IsBlendingEnabled() const;
			inline bool IsColorWriteEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthSortingEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsReflectionMappingEnabled() const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;
			inline bool IsShadowCastingEnabled() const;
			inline bool IsShadowReceiveEnabled() const;

			inline void SetDepthCompareFunc(RendererComparison depthFunc);
			inline void SetDstBlend(BlendFunc func);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetPointSize(float pointSize);
			inline void SetShader(ShaderStageType shaderStage, std::shared_ptr<ShaderStage> shader);
			inline void SetUniformBuffer(std::size_t bufferIndex, UniformBufferRef uniformBuffer);
			inline void SetSrcBlend(BlendFunc func);
			inline void SetTexture(std::size_t textureIndex, std::shared_ptr<Texture> texture);
			inline void SetTextureSampler(std::size_t textureIndex, std::shared_ptr<TextureSampler> sampler);

			// Signals:
			NazaraSignal(OnMaterialRelease, const Material* /*material*/);

		private:
			inline void InvalidatePipeline();
			inline void UpdatePipeline() const;

			struct MaterialTexture
			{
				std::shared_ptr<TextureSampler> sampler;
				std::shared_ptr<Texture> texture;
			};

			std::shared_ptr<const MaterialSettings> m_settings;
			std::vector<MaterialTexture> m_textures;
			std::vector<UniformBufferRef> m_uniformBuffers;
			mutable std::shared_ptr<MaterialPipeline> m_pipeline;
			MaterialPipelineInfo m_pipelineInfo;
			mutable bool m_pipelineUpdated;
			bool m_shadowCastingEnabled;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_BASE_MATERIAL_HPP
