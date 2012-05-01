// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEX_HPP
#define NAZARA_MUTEX_HPP

#define NAZARA_MUTEX

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

class NzMutexImpl;
class NzThreadCondition;

class NAZARA_API NzMutex : NzNonCopyable
{
	friend class NzThreadCondition;

	public:
		NzMutex();
		~NzMutex();

		void Lock();
		bool TryLock();
		void Unlock();

	private:
		NzMutexImpl* m_impl;
};

#undef NAZARA_MUTEX

#endif // NAZARA_MUTEX_HPP
