// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTIMPL_HPP
#define NAZARA_INPUTIMPL_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>

class NzEventImpl
{
	public:
		static NzVector2i GetMousePosition();
		static NzVector2i GetMousePosition(const NzWindow& relativeTo);
		static bool IsKeyPressed(NzKeyboard::Key key);
		static bool IsMouseButtonPressed(NzMouse::Button button);
		static void SetMousePosition(int x, int y);
		static void SetMousePosition(int x, int y, const NzWindow& relativeTo);
};

#endif // NAZARA_INPUTIMPL_HPP
