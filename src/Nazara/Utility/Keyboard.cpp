// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Keyboard.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Utility/Win32/InputImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Utility/Linux/InputImpl.hpp>
#else
	#error Lack of implementation: Keyboard
#endif

#include <Nazara/Utility/Debug.hpp>

NzString NzKeyboard::GetKeyName(Key key)
{
	return NzEventImpl::GetKeyName(key);
}

bool NzKeyboard::IsKeyPressed(Key key)
{
	return NzEventImpl::IsKeyPressed(key);
}
