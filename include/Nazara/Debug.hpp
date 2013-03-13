// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>

#define delete NzMemoryManager::NextFree(__FILE__, __LINE__), delete
#define new new(__FILE__, __LINE__)
