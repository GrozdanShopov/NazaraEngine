// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Un grand merci à Laurent Gomila pour la SFML qui m'aura bien aidé à réaliser cette implémentation

#define OEMRESOURCE

#include <Nazara/Utility/Win32/WindowImpl.hpp>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Cursor.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Icon.hpp>
#include <Nazara/Utility/Win32/CursorImpl.hpp>
#include <Nazara/Utility/Win32/IconImpl.hpp>
#include <cstdio>
#include <memory>
#include <windowsx.h>
#include <Nazara/Utility/Debug.hpp>

#ifdef _WIN64
	#define GCL_HCURSOR GCLP_HCURSOR
	#define GWL_USERDATA GWLP_USERDATA
#endif

// N'est pas défini avec MinGW
#ifndef MAPVK_VK_TO_VSC
	#define MAPVK_VK_TO_VSC 0
#endif

#undef IsMinimized // Conflit avec la méthode du même nom

namespace Nz
{
	namespace
	{
		LPTSTR windowsCursors[] =
		{
			IDC_CROSS,       // WindowCursor_Crosshair
			IDC_ARROW,       // WindowCursor_Default
			IDC_HAND,        // WindowCursor_Hand
			IDC_HAND,        // WindowCursor_Pointer
			IDC_HELP,        // WindowCursor_Help
			IDC_SIZEALL,     // WindowCursor_Move
			nullptr,         // WindowCursor_None
			IDC_APPSTARTING, // WindowCursor_Progress
			IDC_SIZENS,      // WindowCursor_ResizeN
			IDC_SIZENS,      // WindowCursor_ResizeS
			IDC_SIZENWSE,    // WindowCursor_ResizeNW
			IDC_SIZENWSE,    // WindowCursor_ResizeSE
			IDC_SIZENESW,    // WindowCursor_ResizeNE
			IDC_SIZENESW,    // WindowCursor_ResizeSW
			IDC_SIZEWE,      // WindowCursor_ResizeE
			IDC_SIZEWE,      // WindowCursor_ResizeW
			IDC_IBEAM,       // WindowCursor_Text
			IDC_WAIT         // WindowCursor_Wait
		};

		static_assert(sizeof(windowsCursors)/sizeof(LPTSTR) == WindowCursor_Max+1, "Cursor type array is incomplete");

		const wchar_t* className = L"Nazara Window";
		WindowImpl* fullscreenWindow = nullptr;
	}

	WindowImpl::WindowImpl(Window* parent) :
	m_cursor(nullptr),
	m_handle(nullptr),
	m_callback(0),
	m_style(0),
	m_maxSize(-1),
	m_minSize(-1),
	m_parent(parent),
	m_keyRepeat(true),
	m_mouseInside(false),
	m_smoothScrolling(false),
	m_scrolling(0)
	{
	}

	bool WindowImpl::Create(const VideoMode& mode, const String& title, UInt32 style)
	{
		bool fullscreen = (style & WindowStyle_Fullscreen) != 0;
		DWORD win32Style, win32StyleEx;
		unsigned int x, y;
		unsigned int width = mode.width;
		unsigned int height = mode.height;
		if (fullscreen)
		{
			DEVMODE win32Mode;
			std::memset(&win32Mode, 0, sizeof(DEVMODE));
			win32Mode.dmBitsPerPel = mode.bitsPerPixel;
			win32Mode.dmPelsHeight = mode.height;
			win32Mode.dmPelsWidth  = mode.width;
			win32Mode.dmFields	   = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			win32Mode.dmSize	   = sizeof(DEVMODE);

			if (ChangeDisplaySettings(&win32Mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// Situation extrêmement rare grâce à VideoMode::IsValid appelé par Window
				NazaraError("Failed to change display settings for fullscreen, this video mode is not supported by your computer");
				fullscreen = false;
			}
		}

		// Testé une seconde fois car sa valeur peut changer
		if (fullscreen)
		{
			x = 0;
			y = 0;
			win32Style = WS_CLIPCHILDREN | WS_POPUP;

			// Pour cacher la barre des tâches
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
			win32StyleEx = WS_EX_APPWINDOW;

			fullscreenWindow = this;
		}
		else
		{
			win32Style = WS_VISIBLE;
			if (style & WindowStyle_Titlebar)
			{
				win32Style |= WS_CAPTION | WS_MINIMIZEBOX;
				if (style & WindowStyle_Closable)
					win32Style |= WS_SYSMENU;

				if (style & WindowStyle_Resizable)
					win32Style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
			}
			else
				win32Style |= WS_POPUP;

			win32StyleEx = 0;

			RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
			AdjustWindowRect(&rect, win32Style, false);
			width = rect.right-rect.left;
			height = rect.bottom-rect.top;

			x = (GetSystemMetrics(SM_CXSCREEN) - width)/2;
			y = (GetSystemMetrics(SM_CYSCREEN) - height)/2;
		}

		m_callback = 0;

		#if NAZARA_UTILITY_THREADED_WINDOW
		Mutex mutex;
		ConditionVariable condition;
		m_threadActive = true;

		// On attend que la fenêtre soit créée
		mutex.Lock();
		m_thread = Thread(WindowThread, &m_handle, win32StyleEx, title.GetWideString().data(), win32Style, x, y, width, height, this, &mutex, &condition);
		condition.Wait(&mutex);
		mutex.Unlock();
		#else
		m_handle = CreateWindowExW(win32StyleEx, className, title.GetWideString().data(), win32Style, x, y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);
		#endif

		if (!m_handle)
		{
			NazaraError("Failed to create window: " + Error::GetLastSystemError());
			return false;
		}

		if (fullscreen)
		{
			SetForegroundWindow(m_handle);
			ShowWindow(m_handle, SW_SHOW);
		}

		m_eventListener = true;
		m_ownsWindow = true;
		#if !NAZARA_UTILITY_THREADED_WINDOW
		m_sizemove = false;
		#endif
		m_style = style;

		// Récupération de la position/taille de la fenêtre (Après sa création)
		RECT clientRect, windowRect;
		GetClientRect(m_handle, &clientRect);
		GetWindowRect(m_handle, &windowRect);

		m_position.Set(windowRect.left, windowRect.top);
		m_size.Set(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

		return true;
	}

	bool WindowImpl::Create(WindowHandle handle)
	{
		m_handle = static_cast<HWND>(handle);

		if (!m_handle || !IsWindow(m_handle))
		{
			NazaraError("Invalid handle");
			return false;
		}

		m_eventListener = false;
		m_ownsWindow = false;
		#if !NAZARA_UTILITY_THREADED_WINDOW
		m_sizemove = false;
		#endif
		m_style = RetrieveStyle(m_handle);

		RECT clientRect, windowRect;
		GetClientRect(m_handle, &clientRect);
		GetWindowRect(m_handle, &windowRect);

		m_position.Set(windowRect.left, windowRect.top);
		m_size.Set(clientRect.right-clientRect.left, clientRect.bottom-clientRect.top);

		return true;
	}

	void WindowImpl::Destroy()
	{
		if (m_ownsWindow)
		{
			#if NAZARA_UTILITY_THREADED_WINDOW
			if (m_thread.IsJoinable())
			{
				m_threadActive = false;
				PostMessageW(m_handle, WM_NULL, 0, 0); // Pour réveiller le thread

				m_thread.Join();
			}
			#else
			if (m_handle)
				DestroyWindow(m_handle);
			#endif
		}
		else
			SetEventListener(false);
	}

	void WindowImpl::EnableKeyRepeat(bool enable)
	{
		m_keyRepeat = enable;
	}

	void WindowImpl::EnableSmoothScrolling(bool enable)
	{
		m_smoothScrolling = enable;
	}

	WindowHandle WindowImpl::GetHandle() const
	{
		return m_handle;
	}

	unsigned int WindowImpl::GetHeight() const
	{
		return m_size.y;
	}

	Vector2i WindowImpl::GetPosition() const
	{
		return m_position;
	}

	Vector2ui WindowImpl::GetSize() const
	{
		return m_size;
	}

	UInt32 WindowImpl::GetStyle() const
	{
		return m_style;
	}

	String WindowImpl::GetTitle() const
	{
		unsigned int titleSize = GetWindowTextLengthW(m_handle);
		if (titleSize == 0)
			return String();

		titleSize++; // Caractère nul

		std::unique_ptr<wchar_t[]> wTitle(new wchar_t[titleSize]);
		GetWindowTextW(m_handle, wTitle.get(), titleSize);

		return String::Unicode(wTitle.get());
	}

	unsigned int WindowImpl::GetWidth() const
	{
		return m_size.x;
	}

	bool WindowImpl::HasFocus() const
	{
		return GetForegroundWindow() == m_handle;
	}

	void WindowImpl::IgnoreNextMouseEvent(int mouseX, int mouseY)
	{
		// Petite astuce ...
		m_mousePos.x = mouseX;
		m_mousePos.y = mouseY;
	}

	bool WindowImpl::IsMinimized() const
	{
		return IsIconic(m_handle) == TRUE;
	}

	bool WindowImpl::IsVisible() const
	{
		return IsWindowVisible(m_handle) == TRUE;
	}

	void WindowImpl::ProcessEvents(bool block)
	{
		if (m_ownsWindow)
		{
			if (block)
				WaitMessage();

			MSG message;
			while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessageW(&message);
			}
		}
	}

	void WindowImpl::SetCursor(WindowCursor cursor)
	{
		#ifdef NAZARA_DEBUG
		if (cursor > WindowCursor_Max)
		{
			NazaraError("Window cursor out of enum");
			return;
		}
		#endif

		if (cursor != WindowCursor_None)
			m_cursor = static_cast<HCURSOR>(LoadImage(nullptr, windowsCursors[cursor], IMAGE_CURSOR, 0, 0, LR_SHARED));
		else
			m_cursor = nullptr;

		// Pas besoin de libérer le curseur par la suite s'il est partagé
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648045(v=vs.85).aspx
		::SetCursor(m_cursor);
	}

	void WindowImpl::SetCursor(const Cursor& cursor)
	{
		m_cursor = cursor.m_impl->GetCursor();

		::SetCursor(m_cursor);
	}

	void WindowImpl::SetEventListener(bool listener)
	{
		if (m_ownsWindow)
			m_eventListener = listener;
		else if (listener != m_eventListener)
		{
			if (listener)
			{
				SetWindowLongPtr(m_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				m_callback = SetWindowLongPtr(m_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MessageHandler));
				m_eventListener = true;
			}
			else if (m_eventListener)
			{
				SetWindowLongPtr(m_handle, GWLP_WNDPROC, m_callback);
				m_eventListener = false;
			}
		}
	}

	void WindowImpl::SetFocus()
	{
		SetForegroundWindow(m_handle);
	}

	void WindowImpl::SetIcon(const Icon& icon)
	{
		HICON iconHandle = icon.m_impl->GetIcon();

		SendMessage(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(iconHandle));
		SendMessage(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(iconHandle));
	}

	void WindowImpl::SetMaximumSize(int width, int height)
	{
		RECT rect = {0, 0, width, height};
		AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtr(m_handle, GWL_STYLE)), false);

		if (width != -1)
			m_maxSize.x = rect.right-rect.left;
		else
			m_maxSize.x = -1;

		if (height != -1)
			m_maxSize.y = rect.bottom-rect.top;
		else
			m_maxSize.y = -1;
	}

	void WindowImpl::SetMinimumSize(int width, int height)
	{
		RECT rect = {0, 0, width, height};
		AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtr(m_handle, GWL_STYLE)), false);

		if (width != -1)
			m_minSize.x = rect.right-rect.left;
		else
			m_minSize.x = -1;

		if (height != -1)
			m_minSize.y = rect.bottom-rect.top;
		else
			m_minSize.y = -1;
	}

	void WindowImpl::SetPosition(int x, int y)
	{
		SetWindowPos(m_handle, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void WindowImpl::SetSize(unsigned int width, unsigned int height)
	{
		// SetWindowPos demande la taille totale de la fenêtre
		RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
		AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtr(m_handle, GWL_STYLE)), false);

		SetWindowPos(m_handle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	}

	void WindowImpl::SetStayOnTop(bool stayOnTop)
	{
		if (stayOnTop)
			SetWindowPos(m_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		else
			SetWindowPos(m_handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	void WindowImpl::SetTitle(const String& title)
	{
		SetWindowTextW(m_handle, title.GetWideString().data());
	}

	void WindowImpl::SetVisible(bool visible)
	{
		ShowWindow(m_handle, (visible) ? SW_SHOW : SW_HIDE);
	}

	bool WindowImpl::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// Inutile de récupérer des évènements ne venant pas de notre fenêtre
		if (m_handle != window)
			return false;

		switch (message)
		{
			case WM_DESTROY:
				if (fullscreenWindow == this)
					ChangeDisplaySettings(nullptr, 0);

				break;

			case WM_SETCURSOR:
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms648382(v=vs.85).aspx
				if (LOWORD(lParam) == HTCLIENT)
					::SetCursor(m_cursor);

				break;

			case WM_WINDOWPOSCHANGING:
			{
				WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);

				pos->cx = std::max(pos->cx, m_minSize.x);
				pos->cy = std::max(pos->cy, m_minSize.y);

				if (m_maxSize.x >= 0)
					pos->cx = std::min(pos->cx, m_maxSize.x);

				if (m_maxSize.y >= 0)
					pos->cy = std::min(pos->cy, m_maxSize.y);

				break;
			}

			default:
				break;
		}

		if (m_eventListener)
		{
			switch (message)
			{
				case WM_CHAR:
				{
					// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
					bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
					if (m_keyRepeat || !repeated)
					{
						WindowEvent event;
						event.type = WindowEventType_TextEntered;
						event.text.character = static_cast<char32_t>(wParam);
						event.text.repeated = repeated;
						m_parent->PushEvent(event);
					}

					break;
				}

				case WM_CLOSE:
				{
					WindowEvent event;
					event.type = WindowEventType_Quit;
					m_parent->PushEvent(event);

					return true; // Afin que Windows ne ferme pas la fenêtre automatiquement
				}

				#if !NAZARA_UTILITY_THREADED_WINDOW
				case WM_ENTERSIZEMOVE:
				{
					m_sizemove = true;
					break;
				}

				case WM_EXITSIZEMOVE:
				{
					m_sizemove = false;

					// On vérifie ce qui a changé
					RECT clientRect, windowRect;
					GetClientRect(m_handle, &clientRect);
					GetWindowRect(m_handle, &windowRect);

					Vector2i position(windowRect.left, windowRect.top);
					if (m_position != position)
					{
						m_position = position;

						WindowEvent event;
						event.type = WindowEventType_Moved;
						event.position.x = position.x;
						event.position.y = position.y;
						m_parent->PushEvent(event);
					}

					Vector2ui size(clientRect.right-clientRect.left, clientRect.bottom-clientRect.top);
					if (m_size != size)
					{
						m_size = size;

						WindowEvent event;
						event.type = WindowEventType_Resized;
						event.size.width = size.x;
						event.size.height = size.y;
						m_parent->PushEvent(event);
					}
				}
				#endif

				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
				{
					// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
					bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
					if (m_keyRepeat || !repeated)
					{
						WindowEvent event;
						event.type = WindowEventType_KeyPressed;
						event.key.code = ConvertVirtualKey(wParam, lParam);
						event.key.alt = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
						event.key.control = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
						event.key.repeated = repeated;
						event.key.shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
						event.key.system = (((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0));
						m_parent->PushEvent(event);
					}

					break;
				}

				case WM_KEYUP:
				case WM_SYSKEYUP:
				{
					// http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx
					WindowEvent event;
					event.type = WindowEventType_KeyReleased;
					event.key.code = ConvertVirtualKey(wParam, lParam);
					event.key.alt = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
					event.key.control = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
					event.key.shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
					event.key.system = ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000));
					m_parent->PushEvent(event);

					break;
				}

				case WM_KILLFOCUS:
				{
					WindowEvent event;
					event.type = WindowEventType_LostFocus;
					m_parent->PushEvent(event);

					break;
				}

				case WM_LBUTTONDBLCLK:
				{
					// Cet évènement est généré à la place d'un WM_LBUTTONDOWN lors d'un double-clic.
					// Comme nous désirons quand même notifier chaque clic, nous envoyons les deux évènements.
					WindowEvent event;
					event.mouseButton.button = Mouse::Left;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);

					event.type = WindowEventType_MouseButtonDoubleClicked;
					m_parent->PushEvent(event);

					event.type = WindowEventType_MouseButtonPressed;
					m_parent->PushEvent(event);

					break;
				}

				case WM_LBUTTONDOWN:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonPressed;
					event.mouseButton.button = Mouse::Left;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_LBUTTONUP:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonReleased;
					event.mouseButton.button = Mouse::Left;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_MBUTTONDBLCLK:
				{
					WindowEvent event;
					event.mouseButton.button = Mouse::Middle;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);

					event.type = WindowEventType_MouseButtonDoubleClicked;
					m_parent->PushEvent(event);

					event.type = WindowEventType_MouseButtonPressed;
					m_parent->PushEvent(event);

					break;
				}

				case WM_MBUTTONDOWN:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonPressed;
					event.mouseButton.button = Mouse::Middle;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_MBUTTONUP:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonReleased;
					event.mouseButton.button = Mouse::Middle;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				// Nécessite un appel précédent à TrackMouseEvent (Fait dans WM_MOUSEMOVE)
				// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645615(v=vs.85).aspx
				case WM_MOUSELEAVE:
				{
					m_mouseInside = false;

					WindowEvent event;
					event.type = WindowEventType_MouseLeft;
					m_parent->PushEvent(event);
					break;
				}

				case WM_MOUSEMOVE:
				{
					int currentX = GET_X_LPARAM(lParam);
					int currentY = GET_Y_LPARAM(lParam);

					if (!m_mouseInside)
					{
						m_mouseInside = true;

						// On créé un évènement pour être informé de la sortie de la fenêtre
						TRACKMOUSEEVENT mouseEvent;
						mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
						mouseEvent.dwFlags = TME_LEAVE;
						mouseEvent.hwndTrack = m_handle;

						TrackMouseEvent(&mouseEvent);

						WindowEvent event;
						event.type = WindowEventType_MouseEntered;
						m_parent->PushEvent(event);

						event.type = WindowEventType_MouseMoved;

						// Le delta sera 0
						event.mouseMove.deltaX = 0;
						event.mouseMove.deltaY = 0;

						event.mouseMove.x = currentX;
						event.mouseMove.y = currentY;

						m_mousePos.x = currentX;
						m_mousePos.y = currentY;

						m_parent->PushEvent(event);
						break;
					}

					// Si la souris n'a pas bougé (Ou qu'on veut ignorer l'évènement)
					if (m_mousePos.x == currentX && m_mousePos.y == currentY)
						break;

					WindowEvent event;
					event.type = WindowEventType_MouseMoved;
					event.mouseMove.deltaX = currentX - m_mousePos.x;
					event.mouseMove.deltaY = currentY - m_mousePos.y;
					event.mouseMove.x = currentX;
					event.mouseMove.y = currentY;

					m_mousePos.x = currentX;
					m_mousePos.y = currentY;

					m_parent->PushEvent(event);
					break;
				}

				case WM_MOUSEWHEEL:
				{
					if (m_smoothScrolling)
					{
						WindowEvent event;
						event.type = WindowEventType_MouseWheelMoved;
						event.mouseWheel.delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam))/WHEEL_DELTA;
						m_parent->PushEvent(event);
					}
					else
					{
						m_scrolling += GET_WHEEL_DELTA_WPARAM(wParam);
						if (std::abs(m_scrolling) >= WHEEL_DELTA)
						{
							WindowEvent event;
							event.type = WindowEventType_MouseWheelMoved;
							event.mouseWheel.delta = static_cast<float>(m_scrolling/WHEEL_DELTA);
							m_parent->PushEvent(event);

							m_scrolling %= WHEEL_DELTA;
						}
					}
					break;
				}

				case WM_MOVE:
				{
					RECT windowRect;
					GetWindowRect(m_handle, &windowRect);

					WindowEvent event;
					event.type = WindowEventType_Moved;
					event.position.x = windowRect.left;
					event.position.y = windowRect.top;
					m_parent->PushEvent(event);

					break;
				}

				case WM_RBUTTONDBLCLK:
				{
					WindowEvent event;
					event.mouseButton.button = Mouse::Right;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);

					event.type = WindowEventType_MouseButtonDoubleClicked;
					m_parent->PushEvent(event);

					event.type = WindowEventType_MouseButtonPressed;
					m_parent->PushEvent(event);

					break;
				}

				case WM_RBUTTONDOWN:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonPressed;
					event.mouseButton.button = Mouse::Right;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_RBUTTONUP:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonReleased;
					event.mouseButton.button = Mouse::Right;
					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_SETFOCUS:
				{
					WindowEvent event;
					event.type = WindowEventType_GainedFocus;
					m_parent->PushEvent(event);

					break;
				}

				case WM_SIZE:
				{
					#if NAZARA_UTILITY_THREADED_WINDOW
					if (wParam != SIZE_MINIMIZED)
					#else
					if (!m_sizemove && wParam != SIZE_MINIMIZED)
					#endif
					{
						RECT rect;
						GetClientRect(m_handle, &rect);

						Vector2ui size(rect.right-rect.left, rect.bottom-rect.top); // On récupère uniquement la taille de la zone client
						if (m_size == size)
							break;

						m_size = size;

						WindowEvent event;
						event.type = WindowEventType_Resized;
						event.size.width = size.x;
						event.size.height = size.y;
						m_parent->PushEvent(event);
					}
					break;
				}

				case WM_UNICHAR:
				{
					// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646288(v=vs.85).aspx
					if (wParam != UNICODE_NOCHAR)
					{
						bool repeated = ((HIWORD(lParam) & KF_REPEAT) != 0);
						if (m_keyRepeat || !repeated)
						{
							WindowEvent event;
							event.type = WindowEventType_TextEntered;
							event.text.character = static_cast<char32_t>(wParam);
							event.text.repeated = repeated;
							m_parent->PushEvent(event);
						}

						return true;
					}
				}

				case WM_XBUTTONDBLCLK:
				{
					WindowEvent event;
					if (HIWORD(wParam) == XBUTTON1)
						event.mouseButton.button = Mouse::XButton1;
					else
						event.mouseButton.button = Mouse::XButton2;

					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);

					event.type = WindowEventType_MouseButtonDoubleClicked;
					m_parent->PushEvent(event);

					event.type = WindowEventType_MouseButtonPressed;
					m_parent->PushEvent(event);

					break;
				}

				case WM_XBUTTONDOWN:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonPressed;

					if (HIWORD(wParam) == XBUTTON1)
						event.mouseButton.button = Mouse::XButton1;
					else
						event.mouseButton.button = Mouse::XButton2;

					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				case WM_XBUTTONUP:
				{
					WindowEvent event;
					event.type = WindowEventType_MouseButtonReleased;

					if (HIWORD(wParam) == XBUTTON1)
						event.mouseButton.button = Mouse::XButton1;
					else
						event.mouseButton.button = Mouse::XButton2;

					event.mouseButton.x = GET_X_LPARAM(lParam);
					event.mouseButton.y = GET_Y_LPARAM(lParam);
					m_parent->PushEvent(event);

					break;
				}

				default:
					break;
			}
		}

		#if NAZARA_UTILITY_WINDOWS_DISABLE_MENU_KEYS
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646360(v=vs.85).aspx
		if (message == WM_SYSCOMMAND && wParam == SC_KEYMENU)
			return true;
		#endif

		return false;
	}

	bool WindowImpl::Initialize()
	{
		// Nous devons faire un type Unicode pour que la fenêtre le soit également
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx
		WNDCLASSW windowClass;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hbrBackground = nullptr;
		windowClass.hCursor = nullptr; // Le curseur est définit dynamiquement
		windowClass.hIcon = nullptr; // L'icône est définie dynamiquement
		windowClass.hInstance = GetModuleHandle(nullptr);
		windowClass.lpfnWndProc = MessageHandler;
		windowClass.lpszClassName = className;
		windowClass.lpszMenuName = nullptr;
		windowClass.style = CS_DBLCLKS; // Gestion du double-clic

		return RegisterClassW(&windowClass) != 0;
	}

	void WindowImpl::Uninitialize()
	{
		UnregisterClassW(className, GetModuleHandle(nullptr));
	}

	Keyboard::Key WindowImpl::ConvertVirtualKey(WPARAM key, LPARAM flags)
	{
		switch (key)
		{
			case VK_CONTROL:	return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::RControl : Keyboard::LControl;
			case VK_MENU:		return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::RAlt : Keyboard::LAlt;
			case VK_SHIFT:
			{
				static UINT scancode = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
				return (((flags >> 16) & 0xFF) == scancode) ? Keyboard::LShift : Keyboard::RShift;
			}

			case 0x30:                 return Keyboard::Num0;
			case 0x31:                 return Keyboard::Num1;
			case 0x32:                 return Keyboard::Num2;
			case 0x33:                 return Keyboard::Num3;
			case 0x34:                 return Keyboard::Num4;
			case 0x35:                 return Keyboard::Num5;
			case 0x36:                 return Keyboard::Num6;
			case 0x37:                 return Keyboard::Num7;
			case 0x38:                 return Keyboard::Num8;
			case 0x39:                 return Keyboard::Num9;
			case 0x41:                 return Keyboard::A;
			case 0x42:                 return Keyboard::B;
			case 0x43:                 return Keyboard::C;
			case 0x44:                 return Keyboard::D;
			case 0x45:                 return Keyboard::E;
			case 0x46:                 return Keyboard::F;
			case 0x47:                 return Keyboard::G;
			case 0x48:                 return Keyboard::H;
			case 0x49:                 return Keyboard::I;
			case 0x4A:                 return Keyboard::J;
			case 0x4B:                 return Keyboard::K;
			case 0x4C:                 return Keyboard::L;
			case 0x4D:                 return Keyboard::M;
			case 0x4E:                 return Keyboard::N;
			case 0x4F:                 return Keyboard::O;
			case 0x50:                 return Keyboard::P;
			case 0x51:                 return Keyboard::Q;
			case 0x52:                 return Keyboard::R;
			case 0x53:                 return Keyboard::S;
			case 0x54:                 return Keyboard::T;
			case 0x55:                 return Keyboard::U;
			case 0x56:                 return Keyboard::V;
			case 0x57:                 return Keyboard::W;
			case 0x58:                 return Keyboard::X;
			case 0x59:                 return Keyboard::Y;
			case 0x5A:                 return Keyboard::Z;
			case VK_ADD:               return Keyboard::Add;
			case VK_BACK:              return Keyboard::Backspace;
			case VK_BROWSER_BACK:      return Keyboard::Browser_Back;
			case VK_BROWSER_FAVORITES: return Keyboard::Browser_Favorites;
			case VK_BROWSER_FORWARD:   return Keyboard::Browser_Forward;
			case VK_BROWSER_HOME:      return Keyboard::Browser_Home;
			case VK_BROWSER_REFRESH:   return Keyboard::Browser_Refresh;
			case VK_BROWSER_SEARCH:    return Keyboard::Browser_Search;
			case VK_BROWSER_STOP:      return Keyboard::Browser_Stop;
			case VK_CAPITAL:           return Keyboard::CapsLock;
			case VK_CLEAR:             return Keyboard::Clear;
			case VK_DECIMAL:           return Keyboard::Decimal;
			case VK_DELETE:            return Keyboard::Delete;
			case VK_DIVIDE:            return Keyboard::Divide;
			case VK_DOWN:              return Keyboard::Down;
			case VK_END:               return Keyboard::End;
			case VK_ESCAPE:            return Keyboard::Escape;
			case VK_F1:                return Keyboard::F1;
			case VK_F2:                return Keyboard::F2;
			case VK_F3:                return Keyboard::F3;
			case VK_F4:                return Keyboard::F4;
			case VK_F5:                return Keyboard::F5;
			case VK_F6:                return Keyboard::F6;
			case VK_F7:                return Keyboard::F7;
			case VK_F8:                return Keyboard::F8;
			case VK_F9:                return Keyboard::F9;
			case VK_F10:               return Keyboard::F10;
			case VK_F11:               return Keyboard::F11;
			case VK_F12:               return Keyboard::F12;
			case VK_F13:               return Keyboard::F13;
			case VK_F14:               return Keyboard::F14;
			case VK_F15:               return Keyboard::F15;
			case VK_HOME:              return Keyboard::Home;
			case VK_INSERT:            return Keyboard::Insert;
			case VK_LEFT:              return Keyboard::Left;
			case VK_LWIN:              return Keyboard::LSystem;
			case VK_MEDIA_NEXT_TRACK:  return Keyboard::Media_Next;
			case VK_MEDIA_PLAY_PAUSE:  return Keyboard::Media_Play;
			case VK_MEDIA_PREV_TRACK:  return Keyboard::Media_Previous;
			case VK_MEDIA_STOP:        return Keyboard::Media_Stop;
			case VK_MULTIPLY:          return Keyboard::Multiply;
			case VK_NEXT:              return Keyboard::PageDown;
			case VK_NUMPAD0:           return Keyboard::Numpad0;
			case VK_NUMPAD1:           return Keyboard::Numpad1;
			case VK_NUMPAD2:           return Keyboard::Numpad2;
			case VK_NUMPAD3:           return Keyboard::Numpad3;
			case VK_NUMPAD4:           return Keyboard::Numpad4;
			case VK_NUMPAD5:           return Keyboard::Numpad5;
			case VK_NUMPAD6:           return Keyboard::Numpad6;
			case VK_NUMPAD7:           return Keyboard::Numpad7;
			case VK_NUMPAD8:           return Keyboard::Numpad8;
			case VK_NUMPAD9:           return Keyboard::Numpad9;
			case VK_NUMLOCK:           return Keyboard::NumLock;
			case VK_OEM_1:             return Keyboard::Semicolon;
			case VK_OEM_2:             return Keyboard::Slash;
			case VK_OEM_3:             return Keyboard::Tilde;
			case VK_OEM_4:             return Keyboard::LBracket;
			case VK_OEM_5:             return Keyboard::Backslash;
			case VK_OEM_6:             return Keyboard::RBracket;
			case VK_OEM_7:             return Keyboard::Quote;
			case VK_OEM_COMMA:         return Keyboard::Comma;
			case VK_OEM_MINUS:         return Keyboard::Dash;
			case VK_OEM_PERIOD:        return Keyboard::Period;
			case VK_OEM_PLUS:          return Keyboard::Equal;
			case VK_RIGHT:             return Keyboard::Right;
			case VK_PRIOR:             return Keyboard::PageUp;
			case VK_PAUSE:             return Keyboard::Pause;
			case VK_PRINT:             return Keyboard::Print;
			case VK_SCROLL:            return Keyboard::ScrollLock;
			case VK_SNAPSHOT:          return Keyboard::PrintScreen;
			case VK_SUBTRACT:          return Keyboard::Subtract;
			case VK_RETURN:            return Keyboard::Return;
			case VK_RWIN:              return Keyboard::RSystem;
			case VK_SPACE:             return Keyboard::Space;
			case VK_TAB:               return Keyboard::Tab;
			case VK_UP:                return Keyboard::Up;
			case VK_VOLUME_DOWN:       return Keyboard::Volume_Down;
			case VK_VOLUME_MUTE:       return Keyboard::Volume_Mute;
			case VK_VOLUME_UP:         return Keyboard::Volume_Up;

			default:
				return Keyboard::Undefined;
		}
	}

	LRESULT CALLBACK WindowImpl::MessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WindowImpl* me;
		if (message == WM_CREATE)
		{
			me = static_cast<WindowImpl*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtr(window, GWL_USERDATA, reinterpret_cast<LONG_PTR>(me));
		}
		else
			me = reinterpret_cast<WindowImpl*>(GetWindowLongPtr(window, GWL_USERDATA));

		if (me)
		{
			if (me->HandleMessage(window, message, wParam, lParam))
				return 0;
			else if (me->m_callback)
				return CallWindowProcW(reinterpret_cast<WNDPROC>(me->m_callback), window, message, wParam, lParam);
		}

		return DefWindowProcW(window, message, wParam, lParam);
	}

	UInt32 WindowImpl::RetrieveStyle(HWND handle)
	{
		UInt32 style = 0;

		LONG_PTR winStyle = GetWindowLongPtr(handle, GWL_STYLE);

		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
		if (winStyle & WS_CAPTION)
		{
			style |= WindowStyle_Titlebar;
			if (winStyle & WS_SYSMENU)
				style |= WindowStyle_Closable;

			if (winStyle & WS_MAXIMIZEBOX)
				style |= WindowStyle_Resizable;
		}

		if (winStyle & WS_SIZEBOX)
			style |= WindowStyle_Resizable;

		// Pour déterminer si la fenêtre est en plein écran, il suffit de vérifier si elle recouvre l'écran
		DEVMODE mode;
		mode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

		RECT rect;
		if (GetWindowRect(handle, &rect))
		{
			if (static_cast<DWORD>(rect.right-rect.left) == mode.dmPelsWidth && static_cast<DWORD>(rect.bottom-rect.top) == mode.dmPelsHeight)
				style |= WindowStyle_Fullscreen;
		}

		return style;
	}

	#if NAZARA_UTILITY_THREADED_WINDOW
	void WindowImpl::WindowThread(HWND* handle, DWORD styleEx, const wchar_t* title, DWORD style, unsigned int x, unsigned int y, unsigned int width, unsigned int height, WindowImpl* window, Mutex* mutex, ConditionVariable* condition)
	{
		HWND& winHandle = *handle;
		winHandle = CreateWindowExW(styleEx, className, title, style, x, y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), window);

		mutex->Lock();
		condition->Signal();
		mutex->Unlock(); // mutex et condition sont considérés invalides à partir d'ici

		if (!winHandle)
			return;

		while (window->m_threadActive)
			window->ProcessEvents(true);

		DestroyWindow(winHandle);
	}
#endif
}
