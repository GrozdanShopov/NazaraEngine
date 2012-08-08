// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWS_TIME_HPP
#define NAZARA_WINDOWS_TIME_HPP

#include <ctime>
#include <windows.h>

time_t NzFileTimeToTime(FILETIME* time);

#endif // NAZARA_WINDOWS_TIME_HPP
