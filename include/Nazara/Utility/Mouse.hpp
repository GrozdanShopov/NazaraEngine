// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Interface inspir�e de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_MOUSE_HPP
#define NAZARA_MOUSE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>

class NzWindow;

class NAZARA_API NzMouse
{
	public:
		enum Button
		{
			Left,
			Middle,
			Right,
			XButton1,
			XButton2,

			Count
		};

		static NzVector2i GetPosition();
		static NzVector2i GetPosition(const NzWindow& relativeTo);
		static bool IsButtonPressed(Button button);
		static void SetPosition(const NzVector2i& position);
		static void SetPosition(const NzVector2i& position, const NzWindow& relativeTo);
		static void SetPosition(int x, int y);
		static void SetPosition(int x, int y, const NzWindow& relativeTo);
};

#endif // NAZARA_MOUSE_HPP
