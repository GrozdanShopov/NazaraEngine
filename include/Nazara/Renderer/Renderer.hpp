// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_HPP
#define NAZARA_RENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	class Buffer;
	class RendererImpl;

	class NAZARA_RENDERER_API Renderer : public ModuleBase<Renderer>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Platform>;

			struct Config;

			Renderer(Config /*config*/);
			~Renderer();

			inline RendererImpl* GetRendererImpl();

			std::shared_ptr<RenderDevice> InstanciateRenderDevice(std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures = {});

			RenderAPI QueryAPI() const;
			std::string QueryAPIString() const;
			UInt32 QueryAPIVersion() const;

			const std::vector<RenderDeviceInfo>& QueryRenderDevices() const;

			struct Config
			{
				ParameterList customParameters;
				RenderAPI preferredAPI = RenderAPI::Unknown;
#ifdef NAZARA_DEBUG
				RenderAPIValidationLevel validationLevel = RenderAPIValidationLevel::Verbose;
#else
				RenderAPIValidationLevel validationLevel = RenderAPIValidationLevel::Errors;
#endif
			};

		private:
			void LoadBackend(const Config& config);

			std::unique_ptr<RendererImpl> m_rendererImpl;
			Config m_config;
			DynLib m_rendererLib;

			static Renderer* s_instance;
	};
}

#include <Nazara/Renderer/Renderer.inl>

#endif // NAZARA_RENDERER_HPP
