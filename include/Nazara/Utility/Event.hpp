// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspir�e de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_EVENT_HPP
#define NAZARA_EVENT_HPP

#include <Nazara/Utility/Keyboard.hpp>
#include <Nazara/Utility/Mouse.hpp>

struct NzEvent
{
	// Utilis� par:
	// -nzEventType_KeyPressed
	// -nzEventType_KeyReleased
	struct KeyEvent
	{
		NzKeyboard::Key code;
		bool alt;
		bool control;
		bool repeated;
		bool shift;
		bool system;
	};

	// Utilis� par:
	// -nzEventType_MouseButtonDoubleClicked
	// -nzEventType_MouseButtonPressed
	struct MouseButtonEvent
	{
		NzMouse::Button button;
		unsigned int x;
		unsigned int y;
	};

	// Utilis� par:
	// -nzEventType_MouseMoved
	struct MouseMoveEvent
	{
		int deltaX;
		int deltaY;
		unsigned int x;
		unsigned int y;
	};

	// Utilis� par:
	// -nzEventType_MouseWheelMoved
	struct MouseWheelEvent
	{
		float delta;
	};

	// Utilis� par:
	// -nzEventType_Moved
	struct PositionEvent
	{
		int x;
		int y;
	};

	// Utilis� par:
	// -nzEventType_Resized
	struct SizeEvent
	{
		unsigned int height;
		unsigned int width;
	};

	// Utilis� par:
	// -nzEventType_TextEntered
	struct TextEvent
	{
		bool repeated;
		char32_t character;
	};

	nzEventType type;

	union
	{
		// Utilis� par:
		// -nzEventType_KeyPressed
		// -nzEventType_KeyReleased
		KeyEvent key;

		// Utilis� par:
		// -nzEventType_MouseButtonDoubleClicked
		// -nzEventType_MouseButtonPressed
		MouseButtonEvent mouseButton;

		// Utilis� par:
		// -nzEventType_MouseMoved
		MouseMoveEvent mouseMove;

		// Utilis� par:
		// -nzEventType_MouseWheelMoved
		MouseWheelEvent mouseWheel;

		// Utilis� par:
		// -nzEventType_Moved
		PositionEvent position;

		// Utilis� par:
		// -nzEventType_Resized
		SizeEvent size;

		// Utilis� par:
		// -nzEventType_TextEntered
		TextEvent text;
	};
};

#endif // NAZARA_EVENT_HPP
