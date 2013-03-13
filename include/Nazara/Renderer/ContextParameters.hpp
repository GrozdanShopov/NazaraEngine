// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXTPARAMETERS_HPP
#define NAZARA_CONTEXTPARAMETERS_HPP

#include <Nazara/Renderer/RenderTargetParameters.hpp>
#include <Nazara/Utility/VideoMode.hpp>
#include <Nazara/Utility/WindowHandle.hpp>

class NzContext;

struct NAZARA_API NzContextParameters
{
	NzContextParameters(const NzRenderTargetParameters& parameters = NzRenderTargetParameters()) :
	antialiasingLevel(parameters.antialiasingLevel),
	bitsPerPixel(NzVideoMode::GetDesktopMode().bitsPerPixel),
	depthBits(parameters.depthBits),
	majorVersion(defaultMajorVersion),
	minorVersion(defaultMinorVersion),
	stencilBits(parameters.stencilBits),
	shareContext(defaultShareContext),
	window(0),
	compatibilityProfile(defaultCompatibilityProfile),
	debugMode(defaultDebugMode),
	doubleBuffered(defaultDoubleBuffered),
	shared(defaultShared)
	{
	}

	nzUInt8 antialiasingLevel;
	nzUInt8 bitsPerPixel;
	nzUInt8 depthBits;
	nzUInt8 majorVersion;
	nzUInt8 minorVersion;
	nzUInt8 stencilBits;
	const NzContext* shareContext;
	NzWindowHandle window;
	bool compatibilityProfile;
	bool debugMode;
	bool doubleBuffered;
	bool shared;

	static nzUInt8 defaultMajorVersion;
	static nzUInt8 defaultMinorVersion;
	static const NzContext* defaultShareContext;
	static bool defaultCompatibilityProfile;
	static bool defaultDebugMode;
	static bool defaultDoubleBuffered;
	static bool defaultShared;
};

#endif // NAZARA_CONTEXTPARAMETERS_HPP
