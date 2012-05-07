/*
	Nazara Engine

	Copyright (C) 2012 J�r�me "Lynix" Leclercq (Lynix680@gmail.com)
	                   R�mi "overdrivr" B�ges (remi.beges@laposte.net)

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

#ifndef NAZARA_CONFIG_MATH_HPP
#define NAZARA_CONFIG_MATH_HPP

/// Chaque modification d'un param�tre du module n�cessite une recompilation de celui-ci

// D�finit le radian comme l'unit� utilis�e pour les angles
#define NAZARA_MATH_ANGLE_RADIAN 0

// D�finit la disposition des matrices en colonnes
#define NAZARA_MATH_MATRIX_COLUMN_MAJOR 1

// Active les tests de s�curit� bas�s sur le code (Conseill� pour le d�veloppement)
#define NAZARA_MATH_SAFE 1

// Prot�ge le module des acc�s concurrentiels
#define NAZARA_MATH_THREADSAFE 1

#if NAZARA_MATH_THREADSAFE
	#define NAZARA_THREADSAFETY_MATRIX3 1  // NzMatrix3 (COW)
	#define NAZARA_THREADSAFETY_MATRIX4 1  // NzMatrix4 (COW)
#endif

#endif // NAZARA_CONFIG_MATH_HPP
