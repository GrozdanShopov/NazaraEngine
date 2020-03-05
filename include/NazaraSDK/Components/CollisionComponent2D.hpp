// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP
#define NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP

#include <Nazara/Physics2D/Collider2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NazaraSDK/Component.hpp>
#include <memory>

namespace Ndk
{
	class CollisionComponent2D;

	using CollisionComponent2DHandle = Nz::ObjectHandle<CollisionComponent2D>;

	class NDK_API CollisionComponent2D : public Component<CollisionComponent2D>
	{
		friend class ConstraintComponent2D;
		friend class PhysicsComponent2D;
		friend class PhysicsSystem2D;

		public:
			CollisionComponent2D(Nz::Collider2DRef geom = Nz::Collider2DRef());
			CollisionComponent2D(const CollisionComponent2D& collision);
			~CollisionComponent2D() = default;

			Nz::Rectf GetAABB() const;
			const Nz::Collider2DRef& GetGeom() const;
			const Nz::Vector2f& GetGeomOffset() const;

			void Recenter(const Nz::Vector2f& origin);

			void SetGeom(Nz::Collider2DRef geom);
			void SetGeomOffset(const Nz::Vector2f& geomOffset);

			CollisionComponent2D& operator=(Nz::Collider2DRef geom);
			CollisionComponent2D& operator=(CollisionComponent2D&& collision) = default;

			static ComponentIndex componentIndex;

		private:
			void InitializeStaticBody();
			Nz::RigidBody2D* GetRigidBody();
			const Nz::RigidBody2D* GetRigidBody() const;
			Nz::RigidBody2D* GetStaticBody();
			const Nz::RigidBody2D* GetStaticBody() const;

			void OnAttached() override;
			void OnComponentAttached(BaseComponent& component) override;
			void OnComponentDetached(BaseComponent& component) override;
			void OnDetached() override;

			std::unique_ptr<Nz::RigidBody2D> m_staticBody;
			Nz::Collider2DRef m_geom;
			bool m_bodyUpdated;
	};
}

#include <NazaraSDK/Components/CollisionComponent2D.inl>

#endif // NDK_COMPONENTS_COLLISIONCOMPONENT2D_HPP
