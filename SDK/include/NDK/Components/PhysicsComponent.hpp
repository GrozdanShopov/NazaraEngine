// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_PHYSICSCOMPONENT_HPP
#define NDK_COMPONENTS_PHYSICSCOMPONENT_HPP

#include <Nazara/Physics/PhysObject.hpp>
#include <NDK/Component.hpp>
#include <memory>

namespace Ndk
{
	class Entity;

	class NDK_API PhysicsComponent : public Component<PhysicsComponent>
	{
		friend class CollisionComponent;
		friend class PhysicsSystem;

		public:
			PhysicsComponent() = default;
			PhysicsComponent(const PhysicsComponent& physics);
			~PhysicsComponent() = default;

			void AddForce(const Nz::Vector3f& force, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddForce(const Nz::Vector3f& force, const Nz::Vector3f& point, Nz::CoordSys coordSys = Nz::CoordSys_Global);
			void AddTorque(const Nz::Vector3f& torque, Nz::CoordSys coordSys = Nz::CoordSys_Global);

			void EnableAutoSleep(bool autoSleep);

			Nz::Boxf GetAABB() const;
			Nz::Vector3f GetAngularVelocity() const;
			float GetGravityFactor() const;
			float GetMass() const;
			Nz::Vector3f GetMassCenter(Nz::CoordSys coordSys = Nz::CoordSys_Local) const;
			const Nz::Matrix4f& GetMatrix() const;
			Nz::Vector3f GetPosition() const;
			Nz::Quaternionf GetRotation() const;
			Nz::Vector3f GetVelocity() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsSleeping() const;

			void SetAngularVelocity(const Nz::Vector3f& angularVelocity);
			void SetGravityFactor(float gravityFactor);
			void SetMass(float mass);
			void SetMassCenter(const Nz::Vector3f& center);
			void SetPosition(const Nz::Vector3f& position);
			void SetRotation(const Nz::Quaternionf& rotation);
			void SetVelocity(const Nz::Vector3f& velocity);

			static ComponentIndex componentIndex;

		private:
			Nz::PhysObject& GetPhysObject();

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<Nz::PhysObject> m_object;
	};
}

#include <NDK/Components/PhysicsComponent.inl>

#endif // NDK_COMPONENTS_PHYSICSCOMPONENT_HPP
