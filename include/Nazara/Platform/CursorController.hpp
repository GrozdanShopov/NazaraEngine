// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_CURSORCONTROLLER_HPP
#define NAZARA_PLATFORM_CURSORCONTROLLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Enums.hpp>

namespace Nz
{
	class CursorController;

	using CursorControllerHandle = ObjectHandle<CursorController>;

	class CursorController : public HandledObject<CursorController>
	{
		public:
			CursorController() = default;
			CursorController(const CursorController&) = delete;
			CursorController(CursorController&&) noexcept = default;
			~CursorController() = default;

			inline void UpdateCursor(const std::shared_ptr<Cursor>& cursor);

			CursorController& operator=(const CursorController&) = delete;
			CursorController& operator=(CursorController&&) noexcept = default;

			NazaraSignal(OnCursorUpdated, const CursorController* /*cursorController*/, const std::shared_ptr<Cursor>& /*cursor*/);
	};
}

#include <Nazara/Platform/CursorController.inl>

#endif // NAZARA_PLATFORM_CURSORCONTROLLER_HPP
