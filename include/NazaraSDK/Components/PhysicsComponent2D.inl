// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	/*!
	* \brief Constructs a PhysicsComponent2D object by default
	*/
	inline PhysicsComponent2D::PhysicsComponent2D() :
	m_nodeSynchronizationEnabled(true)
	{
	}

	/*!
	* \brief Constructs a PhysicsComponent2D object by copy semantic
	*
	* \param physics PhysicsComponent2D to copy
	*/
	inline PhysicsComponent2D::PhysicsComponent2D(const PhysicsComponent2D& physics) :
	m_nodeSynchronizationEnabled(physics.m_nodeSynchronizationEnabled)
	{
		CopyPhysicsState(*physics.GetRigidBody());
	}

	/*!
	* \brief Applies a physics force to the entity
	*
	* \param force Force to apply on the entity
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddForce(const Nz::Vector2f& force, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, coordSys);
	}

	/*!
	* \brief Applies a physics force to the entity
	*
	* \param force Force to apply on the entity
	* \param point Point where to apply the force
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddForce(const Nz::Vector2f& force, const Nz::Vector2f& point, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddForce(force, point, coordSys);
	}
	
	/*!
	* \brief Applies a impulse to the entity
	*
	* \param impulse Impulse to apply on the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	
	inline void PhysicsComponent2D::AddImpulse(const Nz::Vector2f& impulse, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");
		
		m_object->AddImpulse(impulse, coordSys);
	}
	
	/*!
	* \brief Applies a impulse to the entity
	*
	* \param impulse Impulse to apply on the entity
	* \param point Point where the impulse is applied
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	
	inline void PhysicsComponent2D::AddImpulse(const Nz::Vector2f& impulse, const Nz::Vector2f& point, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");
		
		m_object->AddImpulse(impulse, point, coordSys);
	}
	
	/*!
	* \brief Applies a torque to the entity
	*
	* \param torque Torque to apply on the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline void PhysicsComponent2D::AddTorque(const Nz::RadianAnglef& torque)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->AddTorque(torque);
	}

	/*!
	* \brief Finds the closest point on the entity relative to a position
	* \return True if such a point exists (will return false if no collider exists)
	*
	* \param position The starting point which will be used for the query
	* \param closestPoint The closest point on entity surface
	* \param closestDistance The distance between the closest point and the starting point, may be negative if starting point is inside the entity
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent2D::ClosestPointQuery(const Nz::Vector2f& position, Nz::Vector2f* closestPoint, float* closestDistance) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->ClosestPointQuery(position, closestPoint, closestDistance);
	}

	/*!
	* \brief Enables position/rotation synchronization with the NodeComponent
	*
	* By default, at every update of the PhysicsSystem2D, the NodeComponent's position and rotation (if any) will be synchronized with
	* the values of the PhysicsComponent2D. This function allows to enable/disable this behavior on a per-entity basis.
	*
	* \param nodeSynchronization Should synchronization occur between NodeComponent and PhysicsComponent2D
	*/
	inline void PhysicsComponent2D::EnableNodeSynchronization(bool nodeSynchronization)
	{
		m_nodeSynchronizationEnabled = nodeSynchronization;

		if (m_entity)
			m_entity->Invalidate();
	}

	/*!
	TODO
	*/
	inline void PhysicsComponent2D::ForceSleep()
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->ForceSleep();
	}

	/*!
	TODO
	*/
	inline void PhysicsComponent2D::ForEachArbiter(const std::function<void(Nz::Arbiter2D&)>& callback)
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->ForEachArbiter(callback);
	}
	/*!
	* \brief Gets the AABB of the physics object
	* \return AABB of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Rectf PhysicsComponent2D::GetAABB() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAABB();
	}

	/*!
	* \brief Gets the angular damping or moment of inertia of the physics object
	* \return Angular damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*
	* \see GetMomentOfInertia
	*/

	inline float PhysicsComponent2D::GetAngularDamping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAngularDamping();
	}

	/*!
	* \brief Gets the angular velocity of the physics object
	* \return Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::RadianAnglef PhysicsComponent2D::GetAngularVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetAngularVelocity();
	}

	/*!
	* \brief Gets the gravity center of the physics object
	* \return Gravity center of the object
	*
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector2f PhysicsComponent2D::GetCenterOfGravity(Nz::CoordSys coordSys) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMassCenter(coordSys);
	}

	/*!
	* \brief Gets the elasticity of a shape belonging to this physics object
	* \return Elasticity of the shape
	*
	* \param shapeIndex Shape index of the collider we're interested
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline float PhysicsComponent2D::GetElasticity(std::size_t shapeIndex) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetElasticity(shapeIndex);
	}

	/*!
	* \brief Gets the friction of a shape belonging to this physics object
	* \return Friction of the shape
	*
	* \param shapeIndex Shape index of the collider we're interested
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline float PhysicsComponent2D::GetFriction(std::size_t shapeIndex) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetFriction(shapeIndex);
	}

	/*!
	* \brief Gets the mass of the physics object
	* \return Mass of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline float PhysicsComponent2D::GetMass() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMass();
	}

	/*!
	* \brief Gets the gravity center of the physics object
	* \return Gravity center of the object
	*
	* \param coordSys System coordinates to consider
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector2f PhysicsComponent2D::GetMassCenter(Nz::CoordSys coordSys) const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMassCenter(coordSys);
	}

	/*!
	* \brief Gets the angular damping or moment of inertia of the physics object
	* \return Moment of inertia of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*
	* \see GetAngularDamping
	*/

	inline float PhysicsComponent2D::GetMomentOfInertia() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetMomentOfInertia();
	}

	/*!
	* \brief Gets the position of the physics object
	* \return Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/

	inline Nz::Vector2f PhysicsComponent2D::GetPosition() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetPosition();
	}

	/*!
	* \brief Gets the rotation of the physics object
	* \return Rotation of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::RadianAnglef PhysicsComponent2D::GetRotation() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetRotation();
	}

	/*!
	* \brief Gets the surface velocity of a shape belonging to this physics object
	* \return Surface velocity of the shape
	*
	* \param shapeIndex Shape index of the collider we're interested
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Vector2f PhysicsComponent2D::GetSurfaceVelocity(std::size_t shapeIndex) const
{
		return m_object->GetSurfaceVelocity(shapeIndex);
	}

	/*!
	* \brief Gets the rotation of the physics object
	* \return Shape count of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline std::size_t PhysicsComponent2D::GetShapeCount() const
	{
		return m_object->GetShapeCount();
	}

	/*!
	* \brief Gets the velocity of the physics object
	* \return Velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline Nz::Vector2f PhysicsComponent2D::GetVelocity() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetVelocity();
	}

	/*!
	* \brief Gets the custom velocity function of the physics object
	* \return Velocity function of the object (may be empty if default function is used)
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline auto PhysicsComponent2D::GetVelocityFunction() const -> const VelocityFunc&
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->GetVelocityFunction();
	}

	/*!
	* \brief Checks if position & rotation are synchronized with NodeComponent
	* \return true If synchronization is enabled
	*
	* \see EnableNodeSynchronization
	*/
	inline bool PhysicsComponent2D::IsNodeSynchronizationEnabled() const
	{
		return m_nodeSynchronizationEnabled;
	}

	/*!
	* \brief Checks whether the entity is currently sleeping
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline bool PhysicsComponent2D::IsSleeping() const
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->IsSleeping();
	}

	/*!
	* \brief Checks if this component is bound to a valid rigid body
	*
	* A component may not be bound to a rigid body if the component is not bound to an entity or if this entity is being destroyed
	* 
	* \return true If bound, false otherwise
	*/
	inline bool PhysicsComponent2D::IsValid() const
	{
		return bool(m_object);
	}

	/*!
	* \brief Reset velocity function to default one
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent2D::ResetVelocityFunction()
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->ResetVelocityFunction();
	}

	/*!
	* \brief Sets the angular damping or moment of inertia of the physics object
	*
	* \param angularDamping Angular damping of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*
	* \see SetMomentOfInertia
	*/

	inline void PhysicsComponent2D::SetAngularDamping(float angularDamping)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularDamping(angularDamping);
	}

	/*!
	* \brief Sets the angular velocity of the physics object
	*
	* \param angularVelocity Angular velocity of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent2D::SetAngularVelocity(const Nz::RadianAnglef& angularVelocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetAngularVelocity(angularVelocity);
	}

	/*!
	* \brief Sets the elasticity of the whole physics object
	*
	* Overrides all shapes elasticity with a single value
	*
	* \param elasticity Elasticity to be applied
	*
	* \remark Elasticity must be positive or zero
	*/
	inline void PhysicsComponent2D::SetElasticity(float elasticity)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(elasticity >= 0.f, "Friction must be positive");

		m_object->SetElasticity(elasticity);
	}

	/*!
	* \brief Sets the elasticity of a single shape of the physics object
	*
	* \param shapeIndex Target shape index
	* \param elasticity Elasticity to be applied
	*
	* \remark Elasticity must be positive or zero
	*/
	inline void PhysicsComponent2D::SetElasticity(std::size_t shapeIndex, float elasticity)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(elasticity >= 0.f, "Friction must be positive");

		m_object->SetElasticity(shapeIndex, elasticity);
	}

	/*!
	* \brief Sets the friction of the whole physics object
	*
	* Overrides all shapes friction with a single value
	*
	* \param friction Friction to be applied
	*
	* \remark Friction must be positive or zero
	*/
	inline void PhysicsComponent2D::SetFriction(float friction)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(friction >= 0.f, "Friction must be positive");

		m_object->SetFriction(friction);
	}

	/*!
	* \brief Sets the friction of a single shape of the physics object
	*
	* \param shapeIndex Target shape index
	* \param friction Friction to be applied
	*
	* \remark Friction must be positive or zero
	*/
	inline void PhysicsComponent2D::SetFriction(std::size_t shapeIndex, float friction)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(friction >= 0.f, "Friction must be positive");

		m_object->SetFriction(shapeIndex, friction);
	}

	/*!
	* \brief Sets the mass of the physics object
	*
	* \param mass Mass of the object
	* \param recomputeMoment Should the moment of inertia be recomputed according to the new mass
	*
	* \remark Mass must be positive or zero
	*/
	inline void PhysicsComponent2D::SetMass(float mass, bool recomputeMoment)
	{
		NazaraAssert(m_object, "Invalid physics object");
		NazaraAssert(mass >= 0.f, "Mass should be positive");

		m_object->SetMass(mass, recomputeMoment);
	}

	/*!
	* \brief Sets the gravity center of the physics object
	*
	* \param center Gravity center of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent2D::SetMassCenter(const Nz::Vector2f& center, Nz::CoordSys coordSys)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMassCenter(center, coordSys);
	}

	/*!
	* \brief Sets the angular damping or moment of inertia of the physics object
	*
	* \param moment Moment of inertia of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*
	* \see SetAngularDamping
	*/
	inline void PhysicsComponent2D::SetMomentOfInertia(float moment)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetMomentOfInertia(moment);
	}

	/*!
	* \brief Sets the position of the physics object
	*
	* \param position Position of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent2D::SetPosition(const Nz::Vector2f& position)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetPosition(position);
	}

	/*!
	* \brief Sets the rotation of the physics object
	*
	* \param rotation Rotation of the object
	*
	* \remark Produces a NazaraAssert if the physics object is invalid
	*/
	inline void PhysicsComponent2D::SetRotation(const Nz::RadianAnglef& rotation)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetRotation(rotation);
	}

	/*!
	* \brief Sets the surface velocity of the whole physics object
	*
	* Overrides all shapes surface velocity with a single value
	*
	* \param velocity Surface velocity to be applied
	*/
	inline void PhysicsComponent2D::SetSurfaceVelocity(const Nz::Vector2f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetSurfaceVelocity(velocity);
	}

	/*!
	* \brief Sets the surface velocity of a single shape of the physics object
	*
	* \param shapeIndex Target shape index
	* \param velocity Surface velocity to be applied
	*/
	inline void PhysicsComponent2D::SetSurfaceVelocity(std::size_t shapeIndex, const Nz::Vector2f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetSurfaceVelocity(shapeIndex, velocity);
	}

	/*!
	* \brief Sets the velocity of the physics object
	*
	* \param velocity Velocity of the object
	*/
	inline void PhysicsComponent2D::SetVelocity(const Nz::Vector2f& velocity)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetVelocity(velocity);
	}

	/*!
	* \brief Sets a custom velocity function for the physics object
	*
	* A velocity function is called (for non-kinematic and non-static objects) at every physics update to compute the new velocity of the object.
	* You may call UpdateVelocity (the default velocity function) to let the physics engine compute that itself and then adjust it using GetVelocity/SetVelocity as you need.
	*
	* \param velocityFunc New custom velocity function
	*
	* \remark Passing an empty VelocityFunc has the same effect as calling ResetVelocityFunction
	* \see ResetVelocityFunction
	* \see UpdateVelocity
	*/
	inline void PhysicsComponent2D::SetVelocityFunction(VelocityFunc velocityFunc)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->SetVelocityFunction(std::move(velocityFunc));
	}

	/*!
	* \brief Calls the physics engine default velocity function
	*
	* \param gravity Physics system gravity
	* \param damping Physics system damping (adjusted to deltaTime)
	* \param deltaTime Elapsed time since last physics update
	*/
	inline void PhysicsComponent2D::UpdateVelocity(const Nz::Vector2f& gravity, float damping, float deltaTime)
	{
		NazaraAssert(m_object, "Invalid physics object");

		m_object->UpdateVelocity(gravity, damping, deltaTime);
	}

	/*!
	TODO
	*/
	inline void PhysicsComponent2D::Wakeup()
	{
		NazaraAssert(m_object, "Invalid physics object");

		return m_object->Wakeup();
	}

	inline void PhysicsComponent2D::ApplyPhysicsState(Nz::RigidBody2D& rigidBody) const
	{
		assert(m_pendingStates.valid);

		rigidBody.SetAngularVelocity(m_pendingStates.angularVelocity);
		rigidBody.SetMass(m_pendingStates.mass);
		rigidBody.SetMassCenter(m_pendingStates.massCenter);
		rigidBody.SetMomentOfInertia(m_pendingStates.momentOfInertia);
		rigidBody.SetVelocity(m_pendingStates.velocity);
		rigidBody.SetVelocityFunction(m_pendingStates.velocityFunc);

		for (std::size_t i = 0; i < m_pendingStates.shapes.size(); ++i)
		{
			auto& shapeData = m_pendingStates.shapes[i];
			rigidBody.SetElasticity(i, shapeData.elasticity);
			rigidBody.SetFriction(i, shapeData.friction);
			rigidBody.SetSurfaceVelocity(i, shapeData.surfaceVelocity);
		}
	}

	inline void PhysicsComponent2D::CopyPhysicsState(const Nz::RigidBody2D& rigidBody)
	{
		m_pendingStates.valid = true;

		m_pendingStates.angularVelocity = rigidBody.GetAngularVelocity();
		m_pendingStates.mass = rigidBody.GetMass();
		m_pendingStates.massCenter = rigidBody.GetMassCenter();
		m_pendingStates.momentOfInertia = rigidBody.GetMomentOfInertia();
		m_pendingStates.velocity = rigidBody.GetVelocity();
		m_pendingStates.velocityFunc = rigidBody.GetVelocityFunction();

		m_pendingStates.shapes.resize(rigidBody.GetShapeCount());
		for (std::size_t i = 0; i < m_pendingStates.shapes.size(); ++i)
		{
			auto& shapeData = m_pendingStates.shapes[i];
			shapeData.elasticity = rigidBody.GetElasticity(i);
			shapeData.friction = rigidBody.GetFriction(i);
			shapeData.surfaceVelocity = rigidBody.GetSurfaceVelocity(i);
		}
	}

	/*!
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/
	inline Nz::RigidBody2D* PhysicsComponent2D::GetRigidBody()
	{
		return m_object.get();
	}

	inline const Nz::RigidBody2D* PhysicsComponent2D::GetRigidBody() const
	{
		return m_object.get();
	}
}
