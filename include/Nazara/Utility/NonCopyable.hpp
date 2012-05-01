// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NONCOPYABLE_HPP
#define NAZARA_NONCOPYABLE_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzNonCopyable
{
	protected:
		NzNonCopyable() = default;
		NzNonCopyable(const NzNonCopyable&) = delete;
		NzNonCopyable& operator=(const NzNonCopyable&) = delete;
};

#endif // NAZARA_NONCOPYABLE_HPP
