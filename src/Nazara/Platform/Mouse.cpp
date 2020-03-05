// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/Window.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Platform/Win32/InputImpl.hpp>
#elif defined(NAZARA_PLATFORM_X11)
	#include <Nazara/Platform/X11/InputImpl.hpp>
#else
	#error Lack of implementation: Mouse
#endif

#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	Vector2i Mouse::GetPosition()
	{
		return EventImpl::GetMousePosition();
	}

	Vector2i Mouse::GetPosition(const Window& relativeTo)
	{
		return EventImpl::GetMousePosition(relativeTo);
	}

	bool Mouse::IsButtonPressed(Button button)
	{
		return EventImpl::IsMouseButtonPressed(button);
	}

	void Mouse::SetPosition(const Vector2i& position)
	{
		EventImpl::SetMousePosition(position.x, position.y);
	}

	void Mouse::SetPosition(const Vector2i& position, const Window& relativeTo, bool ignoreEvent)
	{
		if (ignoreEvent && position.x > 0 && position.y > 0)
			relativeTo.IgnoreNextMouseEvent(position.x, position.y);

		EventImpl::SetMousePosition(position.x, position.y, relativeTo);
	}

	void Mouse::SetPosition(int x, int y)
	{
		EventImpl::SetMousePosition(x, y);
	}

	void Mouse::SetPosition(int x, int y, const Window& relativeTo, bool ignoreEvent)
	{
		if (ignoreEvent && x > 0 && y > 0)
			relativeTo.IgnoreNextMouseEvent(x, y);

		EventImpl::SetMousePosition(x, y, relativeTo);
	}
}
