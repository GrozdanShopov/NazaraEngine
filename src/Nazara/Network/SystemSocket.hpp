// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#ifdef NAZARA_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <sys/socket.h>
#endif
