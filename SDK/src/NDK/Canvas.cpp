// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Canvas.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>
#include <limits>

namespace Ndk
{
	void Ndk::Canvas::ResizeToContent()
	{
	}

	void Canvas::Layout()
	{
		if (m_backgroundEntity)
		{
			NodeComponent& node = m_backgroundEntity->GetComponent<NodeComponent>();
			node.SetPosition(-m_padding.left, -m_padding.top);

			m_backgroundSprite->SetSize(m_contentSize.x + m_padding.left + m_padding.right, m_contentSize.y + m_padding.top + m_padding.bottom);
		}
	}

	void Canvas::NotifyWidgetUpdate(std::size_t index)
	{
		WidgetBox& entry = m_widgetBoxes[index];

		Nz::Vector3f pos  = entry.widget->GetPosition();
		Nz::Vector2f size = entry.widget->GetContentSize();

		entry.box.Set(pos.x, pos.y, pos.z, size.x, size.y, 1.f);
	}

	std::size_t Canvas::RegisterWidget(BaseWidget* widget)
	{
		WidgetBox box;
		box.widget = widget;

		std::size_t index = m_widgetBoxes.size();
		m_widgetBoxes.emplace_back(box);

		NotifyWidgetUpdate(index);
		return index;
	}

	void Canvas::UnregisterWidget(std::size_t index)
	{
		if (m_widgetBoxes.size() > 1U)
		{
			WidgetBox& entry = m_widgetBoxes[index];
			WidgetBox& lastEntry = m_widgetBoxes.back();

			entry = std::move(lastEntry);
			entry.widget->UpdateCanvasIndex(index);
			m_widgetBoxes.pop_back();
		}
	}

	void Canvas::OnMouseMoved(const Nz::EventHandler* eventHandler, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		const WidgetBox* bestEntry = nullptr;
		float bestEntryArea = std::numeric_limits<float>::infinity();

		for (const WidgetBox& entry : m_widgetBoxes)
		{
			const Nz::Boxf& box = entry.box;

			if (box.Contains(Nz::Vector3f(event.x, event.y, 0.f)))
			{
				float area = box.width * box.height;
				if (area < bestEntryArea)
				{
					bestEntry = &entry;
					bestEntryArea = area;
				}
			}
		}

		if (bestEntry)
		{
			if (m_hoveredWidget != bestEntry->widget)
			{
				if (m_hoveredWidget)
					m_hoveredWidget->OnMouseExit();

				m_hoveredWidget = bestEntry->widget;
				m_hoveredWidget->OnMouseEnter();
			}

			bestEntry->widget->OnMouseMoved(event.x - bestEntry->box.x, event.y - bestEntry->box.y, event.deltaX, event.deltaY);
		}
		else if (m_hoveredWidget)
		{
			m_hoveredWidget->OnMouseExit();
			m_hoveredWidget = nullptr;
		}
	}
}