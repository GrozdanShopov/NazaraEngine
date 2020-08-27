// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/ButtonWidget.hpp>
#include <NazaraSDK/Components/GraphicsComponent.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>

namespace Ndk
{
	Nz::Color ButtonWidget::s_color { 74, 74, 74 };
	Nz::Color ButtonWidget::s_cornerColor { 180, 180, 180 };
	Nz::Color ButtonWidget::s_hoverColor { 128, 128, 128 };
	Nz::Color ButtonWidget::s_hoverCornerColor { s_cornerColor };
	Nz::Color ButtonWidget::s_pressColor { s_cornerColor };
	Nz::Color ButtonWidget::s_pressCornerColor { s_color };

	ButtonWidget::ButtonWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_color { s_color },
	m_cornerColor { s_cornerColor },
	m_hoverColor { s_hoverColor },
	m_hoverCornerColor { s_hoverCornerColor },
	m_pressColor { s_pressColor },
	m_pressCornerColor { s_pressCornerColor }
	{
		m_gradientSprite = Nz::Sprite::New();
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_cornerColor);
		m_gradientSprite->SetMaterial(Nz::Material::New("Basic2D"));

		m_gradientEntity = CreateEntity();
		m_gradientEntity->AddComponent<NodeComponent>().SetParent(this);
		m_gradientEntity->AddComponent<GraphicsComponent>().Attach(m_gradientSprite);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite, 1);

		Layout();
	}

	const Nz::Color& ButtonWidget::GetDefaultColor()
	{
		return s_color;
	}

	const Nz::Color& ButtonWidget::GetDefaultCornerColor()
	{
		return s_cornerColor;
	}

	const Nz::Color& ButtonWidget::GetDefaultHoverColor()
	{
		return s_hoverColor;
	}

	const Nz::Color& ButtonWidget::GetDefaultHoverCornerColor()
	{
		return s_hoverCornerColor;
	}

	const Nz::Color& ButtonWidget::GetDefaultPressColor()
	{
		return s_pressColor;
	}

	const Nz::Color& ButtonWidget::GetDefaultPressCornerColor()
	{
		return s_pressCornerColor;
	}

	void ButtonWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f size = GetSize();
		m_gradientSprite->SetSize(size);

		Nz::Boxf textBox = m_textEntity->GetComponent<GraphicsComponent>().GetAABB();
		m_textEntity->GetComponent<NodeComponent>().SetPosition(size.x / 2.f - textBox.width / 2.f, size.y / 2.f - textBox.height / 2.f);
	}

	void ButtonWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			m_gradientSprite->SetColor(m_pressColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_pressCornerColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_pressCornerColor);
			m_gradientSprite->SetTexture(m_pressTexture, false);
		}
	}

	void ButtonWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			m_gradientSprite->SetColor(m_hoverColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_hoverCornerColor);
			m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_hoverCornerColor);
			m_gradientSprite->SetTexture(m_hoverTexture, false);

			OnButtonTrigger(this);
		}
	}

	void ButtonWidget::OnMouseEnter()
	{
		m_gradientSprite->SetColor(m_hoverColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_hoverCornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_hoverCornerColor);
		m_gradientSprite->SetTexture(m_hoverTexture, false);
	}

	void ButtonWidget::OnMouseExit()
	{
		m_gradientSprite->SetColor(m_color);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_LeftBottom, m_cornerColor);
		m_gradientSprite->SetCornerColor(Nz::RectCorner_RightBottom, m_cornerColor);
		m_gradientSprite->SetTexture(m_texture, false);
	}
}
