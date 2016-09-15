// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_LABELWIDGET_HPP
#define NDK_WIDGETS_LABELWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Ndk
{
	class World;

	class NDK_API LabelWidget : public BaseWidget
	{
		public:
			LabelWidget(BaseWidget* parent = nullptr);
			LabelWidget(const LabelWidget&) = delete;
			LabelWidget(LabelWidget&&) = default;
			~LabelWidget() = default;

			//virtual LabelWidget* Clone() const = 0;

			void ResizeToContent();

			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);

			LabelWidget& operator=(const LabelWidget&) = delete;
			LabelWidget& operator=(LabelWidget&&) = default;

		private:
			EntityHandle m_textEntity;
			Nz::TextSpriteRef m_textSprite;
	};
}

#include <NDK/Widgets/LabelWidget.inl>

#endif // NDK_WIDGETS_LABELWIDGET_HPP
