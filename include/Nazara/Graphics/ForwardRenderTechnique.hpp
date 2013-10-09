// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERTECHNIQUE_HPP
#define NAZARA_FORWARDRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Graphics/LightManager.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

class NAZARA_API NzForwardRenderTechnique : public NzAbstractRenderTechnique
{
	public:
		NzForwardRenderTechnique();
		~NzForwardRenderTechnique();

		void Clear(const NzScene* scene);
		bool Draw(const NzScene* scene);

		unsigned int GetMaxLightsPerObject() const;
		NzAbstractRenderQueue* GetRenderQueue() override;
		nzRenderTechniqueType GetType() const override;

		void SetMaxLightsPerObject(unsigned int lightCount);

	private:
		void DrawOpaqueModels(const NzScene* scene);
		void DrawSprites(const NzScene* scene);
		void DrawTransparentModels(const NzScene* scene);

		NzForwardRenderQueue m_renderQueue;
		NzIndexBufferRef m_indexBuffer;
		NzLightManager m_directionalLights;
		NzLightManager m_lights;
		NzVertexBuffer m_spriteBuffer;
		unsigned int m_maxLightsPerObject;
};

#endif // NAZARA_FORWARDRENDERTECHNIQUE_HPP
