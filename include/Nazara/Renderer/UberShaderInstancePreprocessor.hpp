// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP
#define NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/UberShaderInstance.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API UberShaderInstancePreprocessor : public UberShaderInstance
	{
		public:
			UberShaderInstancePreprocessor(const Shader* shader);
			virtual ~UberShaderInstancePreprocessor();

			bool Activate() const override;
	};
}

#endif // NAZARA_UBERSHADERINSTANCEPREPROCESSOR_HPP
