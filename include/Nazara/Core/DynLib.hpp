// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#define NAZARA_DYNLIB_EXTENSION ".dll"
#elif defined(NAZARA_PLATFORM_LINUX)
	#define NAZARA_DYNLIB_EXTENSION ".so"
#elif defined(NAZARA_PLATFORM_MACOSX)
	#define NAZARA_DYNLIB_EXTENSION ".dynlib"
#else
	#error OS not handled
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DYNLIB
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

using NzDynLibFunc =  int (*)(); // Type "générique" de pointeur sur fonction

class NzDynLibImpl;

class NzDynLib : NzNonCopyable
{
	friend NzDynLibImpl;

	public:
		NzDynLib(const NzString& libraryPath);
		~NzDynLib();

		NzString GetLastError() const;
		NzDynLibFunc GetSymbol(const NzString& symbol) const;
		bool Load();
		void Unload();

	private:
		void SetLastError(const NzString& error);

		NazaraMutexAttrib(m_mutex, mutable)

		NzString m_lastError;
		NzString m_path;
		NzDynLibImpl* m_impl;
};

#endif // NAZARA_DYNLIB_HPP
