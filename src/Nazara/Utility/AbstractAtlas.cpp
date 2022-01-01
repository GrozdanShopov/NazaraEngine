// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	AbstractAtlas::~AbstractAtlas()
	{
		OnAtlasRelease(this);
	}
}
