/*
	Nazara Engine - Renderer module

	Copyright (C) 2013 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)

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

#ifndef NAZARA_CONFIG_RENDERER_HPP
#define NAZARA_CONFIG_RENDERER_HPP

/// Chaque modification d'un paramètre du module nécessite une recompilation de celui-ci

// Le nombre maximum d'instances pouvant être géré par le Renderer
#define NAZARA_RENDERER_MAX_INSTANCES 8192

// Utilise un tracker pour repérer les éventuels leaks (Ralentit l'exécution)
#define NAZARA_RENDERER_MEMORYLEAKTRACKER 0

// Active le paramère debug des paramètres des contextes par défaut (Perte de performances mais capable de recevoir des messages d'OpenGL)
#define NAZARA_RENDERER_OPENGL_DEBUG 0

// Active les tests de sécurité basés sur le code (Conseillé pour le développement)
#define NAZARA_RENDERER_SAFE 1

// Le nombre maximum de lumières qu'un forward shader supportera
#define NAZARA_RENDERER_SHADER_MAX_LIGHTCOUNT 8

#endif // NAZARA_CONFIG_MODULENAME_HPP
