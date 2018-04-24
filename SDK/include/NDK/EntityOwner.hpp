// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_ENTITYOWNER_HPP
#define NDK_ENTITYOWNER_HPP

#include <NDK/Entity.hpp>

namespace Ndk
{
	class EntityOwner : public EntityHandle
	{
		public:
			EntityOwner() = default;
			explicit EntityOwner(Entity* entity);
			EntityOwner(const EntityOwner& handle) = delete;
			EntityOwner(EntityOwner&& handle) noexcept = default;
			~EntityOwner();

			void Reset(Entity* entity = nullptr);
			void Reset(EntityOwner&& handle);

			EntityOwner& operator=(Entity* entity);
			EntityOwner& operator=(const EntityOwner& handle) = delete;
			EntityOwner& operator=(EntityOwner&& handle) noexcept;
	};
}

#include <NDK/EntityOwner.inl>

#endif // NDK_ENTITYOWNER_HPP
