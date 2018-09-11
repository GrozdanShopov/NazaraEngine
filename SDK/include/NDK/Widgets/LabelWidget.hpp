// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_LABELWIDGET_HPP
#define NDK_WIDGETS_LABELWIDGET_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Nz
{
	class AbstractTextDrawer;
}

namespace Ndk
{
	class NDK_API LabelWidget : public BaseWidget
	{
		public:
			LabelWidget(BaseWidget* parent);
			LabelWidget(const LabelWidget&) = delete;
			LabelWidget(LabelWidget&&) = default;
			~LabelWidget() = default;

			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);

			LabelWidget& operator=(const LabelWidget&) = delete;
			LabelWidget& operator=(LabelWidget&&) = default;

		private:
			void Layout() override;

			EntityHandle m_textEntity;
			Nz::TextSpriteRef m_textSprite;
	};
}

#include <NDK/Widgets/LabelWidget.inl>

#endif // NDK_WIDGETS_LABELWIDGET_HPP
