// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/SDL2/CursorImpl.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	bool Cursor::Create(const Image& cursor, const Vector2i& hotSpot, SystemCursor placeholder)
	{
		Destroy();

		std::unique_ptr<CursorImpl> impl(new CursorImpl);
		if (!impl->Create(cursor, hotSpot.x, hotSpot.y))
		{
			NazaraError("Failed to create cursor implementation");
			return false;
		}

		m_cursorImage = cursor;
		m_impl = impl.release();
		m_systemCursor = placeholder;

		return true;
	}

	void Cursor::Destroy()
	{
		m_cursorImage.Destroy();

		if (m_impl)
		{
			m_impl->Destroy();

			delete m_impl;
			m_impl = nullptr;
		}
	}

	bool Cursor::Create(SystemCursor cursor)
	{
		Destroy();

		std::unique_ptr<CursorImpl> impl(new CursorImpl);
		if (!impl->Create(cursor))
		{
			NazaraError("Failed to create cursor implementation");
			return false;
		}

		m_impl = impl.release();
		m_systemCursor = cursor;

		return true;
	}

	bool Cursor::Initialize()
	{
		if (!CursorImpl::Initialize())
			return false;

		for (std::size_t i = 0; i <= SystemCursor_Max; ++i)
			s_systemCursors[i].Create(static_cast<SystemCursor>(i));

		return true;
	}

	void Cursor::Uninitialize()
	{
		for (Cursor& cursor : s_systemCursors)
			cursor.Destroy();

		CursorImpl::Uninitialize();
	}

	std::array<Cursor, SystemCursor_Max + 1> Cursor::s_systemCursors;
}
