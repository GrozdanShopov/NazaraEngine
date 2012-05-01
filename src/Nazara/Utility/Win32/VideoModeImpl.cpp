// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VideoModeImpl.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <algorithm>
#include <windows.h>
#include <Nazara/Utility/Debug.hpp>

NzVideoMode NzVideoModeImpl::GetDesktopMode()
{
	DEVMODE mode;
	mode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	return NzVideoMode(mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel);
}

void NzVideoModeImpl::GetFullscreenModes(std::vector<NzVideoMode>& modes)
{
	DEVMODE win32Mode;
	win32Mode.dmSize = sizeof(DEVMODE);
	for (unsigned int i = 0; EnumDisplaySettings(nullptr, i, &win32Mode); ++i)
	{
		NzVideoMode mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel);

		// Il existe plusieurs modes avec ces trois caract�ristques identiques
		if (std::find(modes.begin(), modes.end(), mode) == modes.end())
			modes.push_back(mode);
	}
}
