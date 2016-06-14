// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTTEXTDRAWER_HPP
#define NAZARA_ABSTRACTTEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class AbstractImage;
	class Font;

	class NAZARA_UTILITY_API AbstractTextDrawer
	{
		public:
			struct Glyph;

			AbstractTextDrawer() = default;
			virtual ~AbstractTextDrawer();

			virtual const Recti& GetBounds() const = 0;
			virtual Font* GetFont(std::size_t index) const = 0;
			virtual std::size_t GetFontCount() const = 0;
			virtual const Glyph& GetGlyph(std::size_t index) const = 0;
			virtual std::size_t GetGlyphCount() const = 0;

			struct Glyph
			{
				Color color;
				Rectui atlasRect;
				Vector2f corners[4];
				AbstractImage* atlas;
				bool flipped;
			};
	};
}

#endif // NAZARA_ABSTRACTTEXTDRAWER_HPP
