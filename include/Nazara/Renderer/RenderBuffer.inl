// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Renderer/Debug.hpp>

template<typename... Args>
NzRenderBufferRef NzRenderBuffer::New(Args&&... args)
{
	std::unique_ptr<NzRenderBuffer> object(new NzRenderBuffer(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Renderer/DebugOff.hpp>
