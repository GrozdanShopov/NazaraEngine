// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERTECHNIQUE_HPP
#define NAZARA_DEFERREDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/DeferredProxyRenderQueue.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <map>
#include <memory>

namespace Nz
{
	class DeferredRenderPass;

	class NAZARA_GRAPHICS_API DeferredRenderTechnique : public AbstractRenderTechnique
	{
		friend class Graphics;

		public:
			DeferredRenderTechnique();
			~DeferredRenderTechnique();

			void Clear(const SceneData& sceneData) const override;
			bool Draw(const SceneData& sceneData) const override;

			void EnablePass(RenderPassType renderPass, int position, bool enable);

			Texture* GetDepthStencilTexture() const;
			Texture* GetGBuffer(unsigned int i) const;
			RenderTexture* GetGBufferRTT() const;
			const ForwardRenderTechnique* GetForwardTechnique() const;
			DeferredRenderPass* GetPass(RenderPassType renderPass, int position = 0);
			AbstractRenderQueue* GetRenderQueue() override;
			RenderTechniqueType GetType() const override;
			RenderTexture* GetWorkRTT() const;
			Texture* GetWorkTexture(unsigned int i) const;

			bool IsPassEnabled(RenderPassType renderPass, int position);

			DeferredRenderPass* ResetPass(RenderPassType renderPass, int position);

			void SetPass(RenderPassType relativeTo, int position, DeferredRenderPass* pass);

			static bool IsSupported();

		private:
			bool Resize(const Vector2ui& dimensions) const;

			static bool Initialize();
			static void Uninitialize();

			struct RenderPassComparator
			{
				bool operator()(RenderPassType pass1, RenderPassType pass2) const;
			};

			std::map<RenderPassType, std::map<int, std::unique_ptr<DeferredRenderPass>>, RenderPassComparator> m_passes;
			BasicRenderQueue m_deferredRenderQueue; // Must be initialized before the ProxyRenderQueue
			ForwardRenderTechnique m_forwardTechnique; // Must be initialized before the ProxyRenderQueue
			DeferredProxyRenderQueue m_renderQueue;
			mutable TextureRef m_depthStencilTexture;
			mutable RenderTexture m_GBufferRTT;
			mutable RenderTexture m_workRTT;
			mutable TextureRef m_GBuffer[4];
			mutable TextureRef m_workTextures[2];
			mutable Vector2ui m_GBufferSize;
			const RenderTarget* m_viewerTarget;
	};
}

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
