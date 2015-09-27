// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICONIMPL_HPP
#define NAZARA_ICONIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/X11/ScopedXCB.hpp>

namespace Nz
{
	class Image;

	class IconImpl
	{
		public:
			IconImpl();

			bool Create(const Image& image);
			void Destroy();

			xcb_pixmap_t GetIcon();
			xcb_pixmap_t GetMask();

		private:
			XCBPixmap m_iconPixmap;
			XCBPixmap m_maskPixmap;
	};
}

#endif // NAZARA_ICONIMPL_HPP
