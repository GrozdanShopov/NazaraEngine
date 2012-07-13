// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Debug.hpp>

// Version majeure d'OpenGL, initialis� par NzOpenGL::Initialize()
nzUInt8 NzContextParameters::defaultMajorVersion;

 // Version majeure d'OpenGL, initialis� par NzOpenGL::Initialize()
nzUInt8 NzContextParameters::defaultMinorVersion;

// Contexte de partage par d�faut, initialis� par NzOpenGL::Initialize()
const NzContext* NzContextParameters::defaultShareContext = nullptr;

// Si possible, garder la compatibilit� avec les fonctionnalit�s d�pr�ci�es
bool NzContextParameters::defaultCompatibilityProfile = false;

// Mode debug d'OpenGL par d�faut
#if NAZARA_RENDERER_OPENGL_DEBUG || defined(NAZARA_DEBUG)
bool NzContextParameters::defaultDebugMode = true;
#else
bool NzContextParameters::defaultDebugMode = false;
#endif

// Active le double-buffering sur les contextes
bool NzContextParameters::defaultDoubleBuffered = false;

// Active le partage des ressources entre contextes (Via le defaultShareContext)
bool NzContextParameters::defaultShared = true;
