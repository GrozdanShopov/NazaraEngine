// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/ImageWidget.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/GraphicsComponent.hpp>

namespace Ndk
{
	ImageWidget::ImageWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_entity = CreateEntity();
		m_entity->AddComponent<NodeComponent>().SetParent(this);
		auto& gfx = m_entity->AddComponent<GraphicsComponent>();

		m_sprite = Nz::Sprite::New();
		gfx.Attach(m_sprite);
	}

	void ImageWidget::Layout()
	{
		BaseWidget::Layout();

		m_sprite->SetSize(GetSize());
	}
}
