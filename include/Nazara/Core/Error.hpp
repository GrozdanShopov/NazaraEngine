// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERROR_HPP
#define NAZARA_ERROR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if (!(a)) NzError(nzErrorType_AssertFailed, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#else
	#define NazaraAssert(a, err)
#endif

#define NazaraError(err) NzError(nzErrorType_Normal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraInternalError(err) NzError(nzErrorType_Internal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraWarning(err) NzError(nzErrorType_Warning, err, __LINE__, __FILE__, NAZARA_FUNCTION)

enum nzErrorType
{
	nzErrorType_AssertFailed,
	nzErrorType_Internal,
	nzErrorType_Normal,
	nzErrorType_Warning
};

NAZARA_API void NzError(nzErrorType type, const NzString& error, unsigned int line, const char* file, const char* function);
NAZARA_API unsigned int NzGetLastSystemErrorCode();
NAZARA_API NzString NzGetLastSystemError(unsigned int code = NzGetLastSystemErrorCode());

#endif // NAZARA_ERROR_HPP
