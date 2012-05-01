// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIDEOMODEIMPL_HPP
#define NAZARA_VIDEOMODEIMPL_HPP

#include <vector>

class NzVideoMode;

class NzVideoModeImpl
{
	public:
		static NzVideoMode GetDesktopMode();
		static void GetFullscreenModes(std::vector<NzVideoMode>& modes);
};

#endif // NAZARA_VIDEOMODEIMPL_HPP
