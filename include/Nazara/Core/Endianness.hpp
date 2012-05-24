// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENDIANNESS_HPP
#define NAZARA_ENDIANNESS_HPP

#include <Nazara/Prerequesites.hpp>

#if !defined(NAZARA_BIG_ENDIAN) && !defined(NAZARA_LITTLE_ENDIAN)
	// D�tection automatique selon les macros du compilateur
	#if defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || \
		defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__) || defined(__hppa__)
		#define NAZARA_BIG_ENDIAN
	#elif defined(__i386__) || defined(__i386) || defined(__X86__) || defined (__x86_64)
		#define NAZARA_LITTLE_ENDIAN
	#else
		#error Failed to identify endianness, you must define either NAZARA_BIG_ENDIAN or NAZARA_LITTLE_ENDIAN
	#endif
#elif defined(NAZARA_BIG_ENDIAN) && defined(NAZARA_LITTLE_ENDIAN)
	#error You cannot define both NAZARA_BIG_ENDIAN and NAZARA_LITTLE_ENDIAN
#endif

enum nzEndianness
{
	nzEndianness_BigEndian,
	nzEndianness_LittleEndian,
	nzEndianness_Unknown
};

inline void NzByteSwap(void* buffer, unsigned int size);
inline nzEndianness NzGetPlatformEndianness();

#include <Nazara/Core/Endianness.inl>

#endif // NAZARA_ENDIANNESS_HPP
