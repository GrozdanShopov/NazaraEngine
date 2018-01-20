// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD_HPP
#define NAZARA_PHYSWORLD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <unordered_map>

class NewtonBody;
class NewtonJoint;
class NewtonMaterial;
class NewtonWorld;

namespace Nz
{
	class RigidBody3D;

	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		public:
			using BodyIterator = std::function<bool(const RigidBody3D& body)>;
			using AABBOverlapCallback = std::function<bool(const RigidBody3D& firstBody, const RigidBody3D& secondBody)>;
			using CollisionCallback = std::function<bool(const RigidBody3D& firstBody, const RigidBody3D& secondBody)>;

			PhysWorld3D();
			PhysWorld3D(const PhysWorld3D&) = delete;
			PhysWorld3D(PhysWorld3D&&) = delete; ///TODO
			~PhysWorld3D();

			int CreateMaterial(Nz::String name = Nz::String());

			void ForEachBodyInAABB(const Nz::Boxf& box, BodyIterator iterator);

			Vector3f GetGravity() const;
			NewtonWorld* GetHandle() const;
			int GetMaterial(const Nz::String& name);
			float GetStepSize() const;

			void SetGravity(const Vector3f& gravity);
			void SetSolverModel(unsigned int model);
			void SetStepSize(float stepSize);

			void SetMaterialCollisionCallback(int firstMaterial, int secondMaterial, AABBOverlapCallback aabbOverlapCallback, CollisionCallback collisionCallback);
			void SetMaterialDefaultCollidable(int firstMaterial, int secondMaterial, bool collidable);
			void SetMaterialDefaultElasticity(int firstMaterial, int secondMaterial, float elasticCoef);
			void SetMaterialDefaultFriction(int firstMaterial, int secondMaterial, float staticFriction, float kineticFriction);
			void SetMaterialDefaultSoftness(int firstMaterial, int secondMaterial, float softness);
			void SetMaterialSurfaceThickness(int firstMaterial, int secondMaterial, float thickness);

			void Step(float timestep);

			PhysWorld3D& operator=(const PhysWorld3D&) = delete;
			PhysWorld3D& operator=(PhysWorld3D&&) = delete; ///TODO

		private:
			struct Callback
			{
				AABBOverlapCallback aabbOverlapCallback;
				CollisionCallback collisionCallback;
			};

			static int OnAABBOverlap(const NewtonMaterial* const material, const NewtonBody* const body0, const NewtonBody* const body1, int threadIndex);
			static void ProcessContact(const NewtonJoint* const contact, float timestep, int threadIndex);

			std::unordered_map<Nz::UInt64, std::unique_ptr<Callback>> m_callbacks;
			std::unordered_map<Nz::String, int> m_materialIds;
			Vector3f m_gravity;
			NewtonWorld* m_world;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD_HPP
