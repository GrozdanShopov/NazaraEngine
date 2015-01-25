// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEGENERATOR_HPP
#define NAZARA_PARTICLEGENERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>

class NzParticleGenerator;
class NzParticleMapper;
class NzParticleSystem;

using NzParticleGeneratorConstListener = NzObjectListenerWrapper<const NzParticleGenerator>;
using NzParticleGeneratorConstRef = NzObjectRef<const NzParticleGenerator>;
using NzParticleGeneratorListener = NzObjectListenerWrapper<NzParticleGenerator>;
using NzParticleGeneratorRef = NzObjectRef<NzParticleGenerator>;

class NAZARA_API NzParticleGenerator : public NzRefCounted
{
	public:
		NzParticleGenerator() = default;
		NzParticleGenerator(const NzParticleGenerator& generator);
		virtual ~NzParticleGenerator();

		virtual void Generate(NzParticleSystem& system, NzParticleMapper& mapper, unsigned int startId, unsigned int endId) = 0;
};

#endif // NAZARA_PARTICLEGENERATOR_HPP
