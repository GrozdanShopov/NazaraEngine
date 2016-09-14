// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_EVENTHANDLER_HPP
#define NAZARA_EVENTHANDLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Event.hpp>

namespace Nz
{
	class EventHandler
	{
		public:
			EventHandler() = default;
			explicit EventHandler(const EventHandler&);
			EventHandler(EventHandler&&) = default;
			~EventHandler() = default;

			inline void Dispatch(const WindowEvent& event);

			NazaraSignal(OnEvent, const EventHandler* /*eventHandler*/, const WindowEvent& /*event*/);
			NazaraSignal(OnGainedFocus, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnLostFocus, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnKeyPressed, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnKeyReleased, const EventHandler* /*eventHandler*/, const WindowEvent::KeyEvent& /*event*/);
			NazaraSignal(OnMouseButtonDoubleClicked, const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseButtonPressed, const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseButtonReleased, const EventHandler* /*eventHandler*/, const WindowEvent::MouseButtonEvent& /*event*/);
			NazaraSignal(OnMouseEntered, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseLeft, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnMouseMoved, const EventHandler* /*eventHandler*/, const WindowEvent::MouseMoveEvent& /*event*/);
			NazaraSignal(OnMouseWheelMoved, const EventHandler* /*eventHandler*/, const WindowEvent::MouseWheelEvent& /*event*/);
			NazaraSignal(OnMoved, const EventHandler* /*eventHandler*/, const WindowEvent::PositionEvent& /*event*/);
			NazaraSignal(OnQuit, const EventHandler* /*eventHandler*/);
			NazaraSignal(OnResized, const EventHandler* /*eventHandler*/, const WindowEvent::SizeEvent& /*event*/);
			NazaraSignal(OnTextEntered, const EventHandler* /*eventHandler*/, const WindowEvent::TextEvent& /*event*/);
	};
}

#include <Nazara/Utility/EventHandler.inl>

#endif // NAZARA_EVENTHANDLER_HPP
