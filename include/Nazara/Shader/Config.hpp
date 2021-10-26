/*
	Nazara Engine - Shader module

	Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#ifndef NAZARA_SHADER_CONFIG_HPP
#define NAZARA_SHADER_CONFIG_HPP

/// Each modification of a parameter needs a recompilation of the module

// Use the MemoryManager to manage dynamic allocations (can detect memory leak but allocations/frees are slower)
#define NAZARA_SHADER_MANAGE_MEMORY 0

// Activate the security tests based on the code (Advised for development)
#define NAZARA_SHADER_SAFE 1

/// Each modification of a parameter following implies a modification (often minor) of the code

/// Checking the values and types of certain constants
#include <Nazara/Shader/ConfigCheck.hpp>

#if !defined(NAZARA_STATIC)
	#ifdef NAZARA_SHADER_BUILD
		#define NAZARA_SHADER_API NAZARA_EXPORT
	#else
		#define NAZARA_SHADER_API NAZARA_IMPORT
	#endif
#else
	#define NAZARA_SHADER_API
#endif

#endif // NAZARA_SHADER_CONFIG_HPP
