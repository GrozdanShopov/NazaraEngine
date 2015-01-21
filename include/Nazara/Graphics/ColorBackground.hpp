// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLORBACKGROUND_HPP
#define NAZARA_COLORBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/UberShader.hpp>

class NAZARA_API NzColorBackground : public NzAbstractBackground
{
	public:
		NzColorBackground(const NzColor& color = NzColor::Black);

		void Draw(const NzScene* scene) const;

		nzBackgroundType GetBackgroundType() const;
		NzColor GetColor() const;

		void SetColor(const NzColor& color);

	private:
		NzColor m_color;
		NzUberShaderConstRef m_uberShader;
		const NzUberShaderInstance* m_uberShaderInstance;
		int m_materialDiffuseUniform;
		int m_vertexDepthUniform;
};

#endif // NAZARA_COLORBACKGROUND_HPP
