// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_RENDERER_HPP
#define NAZARA_ENUMS_RENDERER_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum class RenderAPI
	{
		Direct3D, ///< Microsoft Render API, only works on MS platforms
		Mantle,   ///< AMD Render API, Vulkan predecessor, only works on AMD GPUs
		Metal,    ///< Apple Render API, only works on OS X platforms
		OpenGL,   ///< Khronos Render API, works on Web/Desktop/Mobile and some consoles
		Vulkan,   ///< New Khronos Render API, made to replace OpenGL, works on desktop (Windows/Linux) and mobile (Android), and Apple platform using MoltenVK

		Unknown,    ///< RenderAPI not corresponding to an entry of the enum, or result of a failed query

		Max = Unknown
	};

	enum class RenderDeviceType
	{
		Integrated, ///< Hardware-accelerated chipset integrated to a CPU (ex: Intel Graphics HD 4000)
		Dedicated,  ///< Hardware-accelerated GPU (ex: AMD R9 390)
		Software,   ///< Software-renderer
		Virtual,    ///< Proxy renderer relaying instructions to another unknown device

		Unknown,    ///< Device type not corresponding to an entry of the enum, or result of a failed query

		Max = Unknown
	};

	enum class ShaderBindingType
	{
		Texture,
		UniformBuffer,

		Max = UniformBuffer
	};

	enum class ShaderLanguage
	{
		GLSL,
		HLSL,
		MSL,
		NazaraBinary,
		SpirV
	};

	enum class QueueType
	{
		Compute,
		Graphics,
		Transfer,

		Max = Transfer
	};

	template<>
	struct EnumAsFlags<QueueType>
	{
		static constexpr QueueType max = QueueType::Max;
	};

	using QueueTypeFlags = Flags<QueueType>;
}

#endif // NAZARA_ENUMS_RENDERER_HPP
