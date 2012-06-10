// Copyright (C) 2012 J�r�me "Lynix" Leclercq (Lynix680@gmail.com)
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Config.hpp>
#if NAZARA_NOISE_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>

	#define delete NzMemoryManager::NextFree(__FILE__, __LINE__), delete
	#define new new(__FILE__, __LINE__)
#endif
