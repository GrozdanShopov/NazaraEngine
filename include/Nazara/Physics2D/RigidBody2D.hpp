// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODY2D_HPP
#define NAZARA_RIGIDBODY2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/Collider2D.hpp>
#include <limits>

struct cpBody;

namespace Nz
{
	class PhysWorld2D;

	class NAZARA_PHYSICS2D_API RigidBody2D
	{
		public:
			RigidBody2D(PhysWorld2D* world, float mass);
			RigidBody2D(PhysWorld2D* world, float mass, Collider2DRef geom);
			RigidBody2D(const RigidBody2D& object);
			RigidBody2D(RigidBody2D&& object);
			~RigidBody2D();

			void AddForce(const Vector2f& force, CoordSys coordSys = CoordSys_Global);
			void AddForce(const Vector2f& force, const Vector2f& point, CoordSys coordSys = CoordSys_Global);
			void AddImpulse(const Vector2f& impulse, CoordSys coordSys = CoordSys_Global);
			void AddImpulse(const Vector2f& impulse, const Vector2f& point, CoordSys coordSys = CoordSys_Global);
			void AddTorque(float torque);

			Rectf GetAABB() const;
			float GetAngularVelocity() const;
			Vector2f GetCenterOfGravity(CoordSys coordSys = CoordSys_Local) const;
			const Collider2DRef& GetGeom() const;
			cpBody* GetHandle() const;
			float GetMass() const;
			float GetMomentOfInertia() const;
			Vector2f GetPosition() const;
			float GetRotation() const;
			std::size_t GetShapeIndex(cpShape* shape) const;
			void* GetUserdata() const;
			Vector2f GetVelocity() const;
			PhysWorld2D* GetWorld() const;

			bool IsKinematic() const;
			bool IsSleeping() const;
			bool IsStatic() const;

			void SetAngularVelocity(float angularVelocity);
			void SetGeom(Collider2DRef geom, bool recomputeMoment = true);
			void SetMass(float mass, bool recomputeMoment = true);
			void SetMassCenter(const Vector2f& center);
			void SetMomentOfInertia(float moment);
			void SetPosition(const Vector2f& position);
			void SetRotation(float rotation);
			void SetStatic(bool setStaticBody = true);
			void SetUserdata(void* ud);
			void SetVelocity(const Vector2f& velocity);

			RigidBody2D& operator=(const RigidBody2D& object);
			RigidBody2D& operator=(RigidBody2D&& object);

			NazaraSignal(OnRigidBody2DMove, RigidBody2D* /*oldPointer*/, RigidBody2D* /*newPointer*/);
			NazaraSignal(OnRigidBody2DRelease, RigidBody2D* /*rigidBody*/);

			static constexpr std::size_t InvalidShapeIndex = std::numeric_limits<std::size_t>::max();

		private:
			cpBody* Create(float mass = 1.f, float moment = 1.f);
			void Destroy();

			static void CopyBodyData(cpBody* from, cpBody* to);

			std::vector<cpShape*> m_shapes;
			Collider2DRef m_geom;
			cpBody* m_handle;
			void* m_userData;
			PhysWorld2D* m_world;
			bool m_isStatic;
			float m_gravityFactor;
			float m_mass;
	};
}

#endif // NAZARA_RIGIDBODY2D_HPP
