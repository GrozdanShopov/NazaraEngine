// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_PHYSICSSYSTEM_HPP
#define NDK_SYSTEMS_PHYSICSSYSTEM_HPP

#include <Nazara/Physics/PhysWorld.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API PhysicsSystem : public System<PhysicsSystem>
	{
		public:
			PhysicsSystem();
			PhysicsSystem(const PhysicsSystem& system);
			~PhysicsSystem() = default;

			Nz::PhysWorld& GetWorld();
			const Nz::PhysWorld& GetWorld() const;

			static SystemIndex systemIndex;

		private:
			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			EntityList m_dynamicObjects;
			EntityList m_staticObjects;
			Nz::PhysWorld m_world;
	};
}

#include <NDK/Systems/PhysicsSystem.inl>

#endif // NDK_SYSTEMS_PHYSICSSYSTEM_HPP
