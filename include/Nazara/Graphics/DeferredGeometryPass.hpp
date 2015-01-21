// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDGEOMETRYPASS_HPP
#define NAZARA_DEFERREDGEOMETRYPASS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/DeferredRenderPass.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>

class NAZARA_API NzDeferredGeometryPass : public NzDeferredRenderPass
{
	public:
		NzDeferredGeometryPass();
		virtual ~NzDeferredGeometryPass();

		bool Process(const NzScene* scene, unsigned int firstWorkTexture, unsigned secondWorkTexture) const;
		bool Resize(const NzVector2ui& dimensions);

	protected:
		struct ShaderUniforms;

		const ShaderUniforms* GetShaderUniforms(const NzShader* shader) const;

		struct ShaderUniforms
		{
			int eyePosition;
			int sceneAmbient;
			int textureOverlay;
		};

		mutable std::unordered_map<const NzShader*, ShaderUniforms> m_shaderUniforms;
		NzRenderStates m_clearStates;
		NzShaderRef m_clearShader;
};

#endif // NAZARA_DEFERREDGEOMETRYPASS_HPP
