// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCOPEDXCB_HPP
#define NAZARA_SCOPEDXCB_HPP

#include <Nazara/Prerequisites.hpp>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <cstdlib>

namespace Nz
{
	class ScopedXCBConnection
	{
		public:
			ScopedXCBConnection();
			~ScopedXCBConnection();

			operator xcb_connection_t*() const;

		private:
			xcb_connection_t* m_connection;
	};

	class ScopedXCBEWMHConnection
	{
		public:
			ScopedXCBEWMHConnection(xcb_connection_t* connection);
			~ScopedXCBEWMHConnection();

			xcb_ewmh_connection_t* operator ->() const;

			operator xcb_ewmh_connection_t*() const;

		private:
			xcb_ewmh_connection_t* m_ewmhConnection;
	};

	template <typename T>
	class ScopedXCB
	{
		public:
			ScopedXCB(T* pointer);
			~ScopedXCB();

			T* operator ->() const;
			T** operator &();

			operator bool() const;

			T* get() const;

		private:
			T* m_pointer;
	};

	class XCBGContext
	{
		public:
			XCBGContext(xcb_connection_t* connection);
			~XCBGContext();

			bool Create(xcb_drawable_t drawable, uint32_t value_mask, const uint32_t* value_list);

			void Destroy();

			operator xcb_gcontext_t() const;

		private:
			xcb_connection_t* m_connection;
			xcb_gcontext_t m_gcontext;
	};
}

#include <Nazara/Renderer/GLX/ScopedXCB.inl>

#endif // NAZARA_SCOPEDXCB_HPP
