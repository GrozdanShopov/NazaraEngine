// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DRAWABLE_HPP
#define NAZARA_DRAWABLE_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzDrawable
{
	public:
		NzDrawable() = default;
		~NzDrawable();

		virtual void Draw() const = 0;
};

#endif // NAZARA_DRAWABLE_HPP
