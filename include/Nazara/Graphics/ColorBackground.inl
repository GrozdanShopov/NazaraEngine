// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

template<typename... Args>
NzColorBackgroundRef NzColorBackground::New(Args&&... args)
{
	std::unique_ptr<NzColorBackground> object(new NzColorBackground(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
