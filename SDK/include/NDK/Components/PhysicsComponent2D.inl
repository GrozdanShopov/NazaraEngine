// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/PhysicsComponent2D.hpp>
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
	inline PhysicsComponent2D::PhysicsComponent2D(const PhysicsComponent2D& physics)
	{
		// No copy of physical object (because we only create it when attached to an entity)
		NazaraUnused(physics);
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
	* \brief Gets the underlying physics object
	* \return A reference to the physics object
	*/
	inline Nz::RigidBody2D* PhysicsComponent2D::GetRigidBody()
	{
		return m_object.get();
	}
}
