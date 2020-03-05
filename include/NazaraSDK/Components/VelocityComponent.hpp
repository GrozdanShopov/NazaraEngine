// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_VELOCITYCOMPONENT_HPP
#define NDK_COMPONENTS_VELOCITYCOMPONENT_HPP

#include <Nazara/Math/Vector3.hpp>
#include <NazaraSDK/Component.hpp>

namespace Ndk
{
	class VelocityComponent;

	using VelocityComponentHandle = Nz::ObjectHandle<VelocityComponent>;

	class NDK_API VelocityComponent : public Component<VelocityComponent>
	{
		public:
			VelocityComponent(const Nz::Vector3f& velocity = Nz::Vector3f::Zero(), Nz::CoordSys coordSystem = Nz::CoordSys_Global);
			~VelocityComponent() = default;

			Nz::Vector3f linearVelocity;
			Nz::CoordSys coordSys;

			VelocityComponent& operator=(const Nz::Vector3f& vel);

			static ComponentIndex componentIndex;
	};
}

#include <NazaraSDK/Components/VelocityComponent.inl>

#endif // NDK_COMPONENTS_VELOCITYCOMPONENT_HPP
