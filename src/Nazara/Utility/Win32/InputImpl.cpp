// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Win32/InputImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Window.hpp>
#include <windows.h>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	int vKeys[NzKeyboard::Count] = {
		// Lettres
		0x41, // Key::A
		0x42, // Key::B
		0x43, // Key::C
		0x44, // Key::D
		0x45, // Key::E
		0x46, // Key::F
		0x47, // Key::G
		0x48, // Key::H
		0x49, // Key::I
		0x4A, // Key::J
		0x4B, // Key::K
		0x4C, // Key::L
		0x4D, // Key::M
		0x4E, // Key::N
		0x4F, // Key::O
		0x50, // Key::P
		0x51, // Key::Q
		0x52, // Key::R
		0x53, // Key::S
		0x54, // Key::T
		0x55, // Key::U
		0x56, // Key::V
		0x57, // Key::W
		0x58, // Key::X
		0x59, // Key::Y
		0x5A, // Key::Z

		// Touches de fonction
		VK_F1,	// Key::F1
		VK_F2,	// Key::F2
		VK_F3,	// Key::F3
		VK_F4,	// Key::F4
		VK_F5,	// Key::F5
		VK_F6,	// Key::F6
		VK_F7,	// Key::F7
		VK_F8,	// Key::F8
		VK_F9,	// Key::F9
		VK_F10,	// Key::F10
		VK_F11,	// Key::F11
		VK_F12,	// Key::F12
		VK_F13,	// Key::F13
		VK_F14,	// Key::F14
		VK_F15,	// Key::F15

		// Flèches directionnelles
		VK_DOWN,  // Key::Down
		VK_LEFT,  // Key::Left
		VK_RIGHT, // Key::Right
		VK_UP,	  // Key::Up

		// Pavé numérique
		VK_ADD,		 // Key::Add
		VK_DIVIDE,	 // Key::Divide
		VK_MULTIPLY, // Key::Multiply
		VK_NUMPAD0,	 // Key::Numpad0
		VK_NUMPAD1,	 // Key::Numpad1
		VK_NUMPAD2,	 // Key::Numpad2
		VK_NUMPAD3,	 // Key::Numpad3
		VK_NUMPAD4,	 // Key::Numpad4
		VK_NUMPAD5,	 // Key::Numpad5
		VK_NUMPAD6,	 // Key::Numpad6
		VK_NUMPAD7,	 // Key::Numpad7
		VK_NUMPAD8,	 // Key::Numpad8
		VK_NUMPAD9,	 // Key::Numpad9
		VK_SUBTRACT, // Key::Subtract

		// Diverss
		VK_OEM_5,	   // Key::Backslash
		VK_BACK,	   // Key::Backspace
		VK_CLEAR,	   // Key::Clear
		VK_OEM_COMMA,  // Key::Comma,
		VK_OEM_MINUS,  // Key::Dash
		VK_DELETE,	   // Key::Delete
		VK_END,		   // Key::End
		VK_OEM_PLUS,   // Key::Equal
		VK_ESCAPE,	   // Key::Escape
		VK_HOME,	   // Key::Home
		VK_INSERT,	   // Key::Insert
		VK_LMENU,	   // Key::LAlt
		VK_OEM_4,	   // Key::LBracket
		VK_LCONTROL,   // Key::LControl
		VK_LSHIFT,	   // Key::LShift
		VK_LWIN,	   // Key::LSystem
		0x30,		   // Key::Num0
		0x31,		   // Key::Num1
		0x32,		   // Key::Num2
		0x33,		   // Key::Num3
		0x34,		   // Key::Num4
		0x35,		   // Key::Num5
		0x36,		   // Key::Num6
		0x37,		   // Key::Num7
		0x38,		   // Key::Num8
		0x39,		   // Key::Num9
		VK_NEXT,	   // Key::PageDown
		VK_PRIOR,	   // Key::PageUp
		VK_PAUSE,	   // Key::Pause
		VK_OEM_PERIOD, // Key::Period,
		VK_PRINT,	   // Key::Print
		VK_SNAPSHOT,   // Key::PrintScreen
		VK_OEM_7,	   // Key::Quote
		VK_RMENU,	   // Key::RAlt
		VK_OEM_6,	   // Key::RBracket
		VK_RCONTROL,   // Key::RControl
		VK_RETURN,	   // Key::Return
		VK_RSHIFT,	   // Key::RShift
		VK_RWIN,	   // Key::RSystem
		VK_OEM_1,	   // Key::Semicolon
		VK_OEM_2,	   // Key::Slash
		VK_SPACE,	   // Key::Space
		VK_TAB,		   // Key::Tab
		VK_OEM_3,	   // Key::Tilde

		// Touches navigateur
		VK_BROWSER_BACK,	  // Key::Browser_Back
		VK_BROWSER_FAVORITES, // Key::Browser_Favorites
		VK_BROWSER_FORWARD,	  // Key::Browser_Forward
		VK_BROWSER_HOME,	  // Key::Browser_Home
		VK_BROWSER_REFRESH,	  // Key::Browser_Refresh
		VK_BROWSER_SEARCH,	  // Key::Browser_Search
		VK_BROWSER_STOP,	  // Key::Browser_Stop

		// Touches de contrôle
		VK_MEDIA_NEXT_TRACK, // Key::Media_Next,
		VK_MEDIA_PLAY_PAUSE, // Key::Media_PlayPause,
		VK_MEDIA_PREV_TRACK, // Key::Media_Previous,
		VK_MEDIA_STOP,		 // Key::Media_Stop,

		// Touches de contrôle du volume
		VK_VOLUME_DOWN,	// Key::Volume_Down
		VK_VOLUME_MUTE,	// Key::Volume_Mute
		VK_VOLUME_UP,	// Key::Volume_Up

		// Touches à verrouillage
		VK_CAPITAL,	// Key::CapsLock
		VK_NUMLOCK,	// Key::NumLock
		VK_SCROLL	// Key::ScrollLock
	};
}

NzString NzEventImpl::GetKeyName(NzKeyboard::Key key)
{
	// http://www.ffuts.org/blog/mapvirtualkey-getkeynametext-and-a-story-of-how-to/
	int vk = vKeys[key];
	unsigned int code = MapVirtualKeyW(vk, 0) << 16;

	///FIXME: Liste complète ?
	switch (vk)
	{
		case VK_ATTN:
		case VK_DOWN:
		case VK_DELETE:
		case VK_DIVIDE:
		case VK_END:
		case VK_HOME:
		case VK_INSERT:
		case VK_LEFT:
		case VK_LWIN:
		case VK_OEM_1:
		case VK_OEM_2:
		case VK_OEM_3:
		case VK_OEM_4:
		case VK_OEM_5:
		case VK_OEM_6:
		case VK_OEM_7:
		case VK_OEM_CLEAR:
		case VK_OEM_COMMA:
		case VK_OEM_MINUS:
		case VK_OEM_PERIOD:
		case VK_OEM_PLUS:
		case VK_PAUSE:
		case VK_NEXT:
		case VK_NUMLOCK:
		case VK_PRIOR:
		case VK_RIGHT:
		case VK_RWIN:
		case VK_UP:
			code |= 0x1000000; // 24ème bit pour l'extension
			break;
	}

	wchar_t keyName[20]; // Je ne pense pas que ça dépassera 20 caractères
	if (!GetKeyNameTextW(code, &keyName[0], 20))
		return "Unknown";

	return NzString::Unicode(keyName);
}

NzVector2i NzEventImpl::GetMousePosition()
{
	POINT pos;
	GetCursorPos(&pos);

	return NzVector2i(pos.x, pos.y);
}

NzVector2i NzEventImpl::GetMousePosition(const NzWindow& relativeTo)
{
	HWND handle = reinterpret_cast<HWND>(relativeTo.GetHandle());
	if (handle)
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(handle, &pos);

		return NzVector2i(pos.x, pos.y);
	}
	else
	{
		NazaraError("Window's handle is invalid");

		// Attention que (-1, -1) est une position tout à fait valide et ne doit pas être utilisée pour tester l'erreur
		return NzVector2i(-1, -1);
	}
}

bool NzEventImpl::IsKeyPressed(NzKeyboard::Key key)
{
	switch (key)
	{
		case NzKeyboard::CapsLock:
		case NzKeyboard::NumLock:
		case NzKeyboard::ScrollLock:
			return GetKeyState(vKeys[key]) != 0;

		default:
			return (GetAsyncKeyState(vKeys[key]) & 0x8000) != 0;
	}
}

bool NzEventImpl::IsMouseButtonPressed(NzMouse::Button button)
{
	static int vButtons[NzMouse::Max+1] = {
		VK_LBUTTON,	 // Button::Left
		VK_MBUTTON,	 // Button::Middle
		VK_RBUTTON,	 // Button::Right
		VK_XBUTTON1, // Button::XButton1
		VK_XBUTTON2	 // Button::XButton2
	};

	// Gestion de l'inversement des boutons de la souris
	switch (button)
	{
		case NzMouse::Left:
			if (GetSystemMetrics(SM_SWAPBUTTON))
				return (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

			break;

		case NzMouse::Right:
			if (GetSystemMetrics(SM_SWAPBUTTON))
				return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

			break;

		default:
			break;
	}

	return (GetAsyncKeyState(vButtons[button]) & 0x8000) != 0;
}

void NzEventImpl::SetMousePosition(int x, int y)
{
	SetCursorPos(x, y);
}

void NzEventImpl::SetMousePosition(int x, int y, const NzWindow& relativeTo)
{
	HWND handle = reinterpret_cast<HWND>(relativeTo.GetHandle());
	if (handle)
	{
		POINT pos = {x, y};
		ClientToScreen(handle, &pos);
		SetCursorPos(pos.x, pos.y);
	}
	else
		NazaraError("Window's handle is invalid");
}
