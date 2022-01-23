// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
#define NAZARA_GRAPHICS_SUBMESHRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class RenderPipeline;
	class ShaderBinding;

	class NAZARA_GRAPHICS_API SubmeshRenderer : public ElementRenderer
	{
		public:
			SubmeshRenderer() = default;
			~SubmeshRenderer() = default;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& currentFrame, const RenderStates& renderStates, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
			void Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
			void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame) override;

		private:
			std::vector<ShaderBinding::Binding> m_bindingCache;
	};

	struct SubmeshRendererData : public ElementRendererData
	{
		struct DrawCall
		{
			const RenderBuffer* indexBuffer;
			const RenderBuffer* vertexBuffer;
			const RenderPipeline* renderPipeline;
			const ShaderBinding* shaderBinding;
			std::size_t indexCount;
			Recti scissorBox;
		};

		std::vector<DrawCall> drawCalls;
		std::vector<ShaderBindingPtr> shaderBindings;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif // NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
