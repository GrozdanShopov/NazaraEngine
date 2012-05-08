// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASHIMPL_HPP
#define NAZARA_HASHIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

class NzHashDigest;

class NAZARA_API NzHashImpl : NzNonCopyable
{
	public:
		NzHashImpl() = default;
		virtual ~NzHashImpl() {}

		virtual void Append(const nzUInt8* data, unsigned int len) = 0;
		virtual void Begin() = 0;
		virtual NzHashDigest End() = 0;
};

#endif // NAZARA_HASHIMPL_HPP
