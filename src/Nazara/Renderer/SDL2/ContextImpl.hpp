// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXTIMPL_HPP
#define NAZARA_CONTEXTIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <SDL2/SDL.h>

namespace Nz
{
	class ContextImpl
	{
		public:
			ContextImpl();

			bool Activate() const;

			bool Create(ContextParameters& parameters);

			void Destroy();

			void EnableVerticalSync(bool enabled);

			void SwapBuffers();

			static bool Desactivate();

		private:
			SDL_GLContext m_context;
			SDL_Window* m_window;
			bool m_ownsWindow;

			static SDL_Window* lastActive;
	};
}

#endif // NAZARA_CONTEXTIMPL_HPP
