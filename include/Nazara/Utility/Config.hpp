/*
	Nazara Engine

	Copyright (C) 2012 J�r�me "Lynix" Leclercq (Lynix680@gmail.com)

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

#ifndef NAZARA_CONFIG_UTILITY_HPP
#define NAZARA_CONFIG_UTILITY_HPP

/// Chaque modification d'un param�tre du module n�cessite une recompilation de celui-ci

// Force les buffers � poss�der un stride multiple de 32 bytes (Gain de performances sur certaines cartes/plus de consommation m�moire)
#define NAZARA_UTILITY_FORCE_DECLARATION_STRIDE_MULTIPLE_OF_32 0

// Utilise un tracker pour rep�rer les �ventuels leaks (Ralentit l'ex�cution)
#define NAZARA_UTILITY_MEMORYLEAKTRACKER 0

// Active les tests de s�curit� bas�s sur le code (Conseill� pour le d�veloppement)
#define NAZARA_UTILITY_SAFE 1

// Fait tourner chaque fen�tre dans un thread s�par� si le syst�me le supporte
#define NAZARA_UTILITY_THREADED_WINDOW 0 ///FIXME: Bugg� depuis GCC 4.7

// Prot�ge les classes des acc�s concurrentiels
#define NAZARA_UTILITY_THREADSAFE 1

// Les classes � prot�ger des acc�s concurrentiels
#define NAZARA_THREADSAFETY_IMAGE             1 // NzImage (COW)
#define NAZARA_THREADSAFETY_VERTEXDECLARATION 1 // NzVertexDeclaration (COW)

#endif // NAZARA_CONFIG_UTILITY_HPP
