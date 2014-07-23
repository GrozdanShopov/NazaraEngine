// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCK_HPP
#define NAZARA_CLOCK_HPP

#include <Nazara/Prerequesites.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_CLOCK
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NAZARA_API NzClock
{
	public:
		NzClock(nzUInt64 startingValue = 0, bool paused = false);
		NzClock(const NzClock& clock) = default;

		float GetSeconds() const;
		nzUInt64 GetMicroseconds() const;
		nzUInt64 GetMilliseconds() const;

		bool IsPaused() const;

		void Pause();
		void Restart();
		void Unpause();

		NzClock& operator=(const NzClock& clock) = default;

	private:
		NazaraMutexAttrib(m_mutex, mutable)

		nzUInt64 m_elapsedTime;
		nzUInt64 m_refTime;
		bool m_paused;
};

typedef nzUInt64 (*NzClockFunction)();

extern NAZARA_API NzClockFunction NzGetMicroseconds;
extern NAZARA_API NzClockFunction NzGetMilliseconds;

#endif // NAZARA_CLOCK_HPP
