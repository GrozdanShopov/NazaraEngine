// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERWINDOW_HPP
#define NAZARA_RENDERWINDOW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/Renderer/RenderWindowParameters.hpp>
#include <Nazara/Platform/Window.hpp>
#include <memory>

namespace Nz
{
	class RenderDevice;

	class NAZARA_RENDERER_API RenderWindow : public Window
	{
		public:
			inline RenderWindow();
			inline RenderWindow(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline explicit RenderWindow(void* handle, const RenderWindowParameters &parameters = RenderWindowParameters());
			inline ~RenderWindow();

			inline bool Create(VideoMode mode, const std::string& title, WindowStyleFlags style = WindowStyle_Default, const RenderWindowParameters& parameters = RenderWindowParameters());
			inline bool Create(void* handle, const RenderWindowParameters &parameters = RenderWindowParameters());

			void Display();

			void EnableVerticalSync(bool enabled);

			inline RenderWindowImpl* GetImpl();
			std::shared_ptr<RenderDevice> GetRenderDevice();
			inline RenderSurface* GetSurface();

			inline bool IsValid() const;

			inline void SetFramerateLimit(unsigned int limit);

			RenderWindow &operator=(const RenderWindow &) = delete;
			RenderWindow &operator=(RenderWindow &&) = delete; ///TODO

		protected:
			bool OnWindowCreated() override;
			void OnWindowDestroy() override;
			void OnWindowResized() override;

		private:
			std::unique_ptr<RenderWindowImpl> m_impl;
			Clock m_clock;
			std::unique_ptr<RenderSurface> m_surface;
			RenderWindowParameters m_parameters;
			unsigned int m_framerateLimit;
		};
} // namespace Nz

#include <Nazara/Renderer/RenderWindow.inl>

#endif // NAZARA_RENDERWINDOW_HPP
