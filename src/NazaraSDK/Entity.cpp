// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Entity.hpp>
#include <NazaraSDK/BaseComponent.hpp>
#include <NazaraSDK/World.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Entity
	* \brief NDK class that represents an entity in a world
	*/

	// Must exists in .cpp file because of BaseComponent unique_ptr
	Entity::Entity(Entity&&) noexcept = default;

	/*!
	* \brief Constructs a Entity object linked to a world and with an id
	*
	* \param world World in which the entity interact
	* \param id Identifier of the entity
	*/
	Entity::Entity(World* world, EntityId id) :
	m_world(world),
	m_id(id)
	{
	}

	/*!
	* \brief Destructs the object and calls Destroy
	*
	* \see Destroy
	*/
	Entity::~Entity()
	{
		if (m_world && m_valid)
			Destroy();
	}

	/*!
	* \brief Adds a component to the entity
	* \return A reference to the newly added component
	*
	* \param componentPtr Component to add to the entity
	*
	* \remark Produces a NazaraAssert if component is nullptr
	*/

	BaseComponent& Entity::AddComponent(std::unique_ptr<BaseComponent>&& componentPtr)
	{
		NazaraAssert(componentPtr, "Component must be valid");

		ComponentIndex index = componentPtr->GetIndex();

		// We ensure that the vector has enough space
		if (index >= m_components.size())
			m_components.resize(index + 1);

		// Affectation and return of the component
		m_components[index] = std::move(componentPtr);
		m_componentBits.UnboundedSet(index);
		m_removedComponentBits.UnboundedReset(index);

		Invalidate();

		// We get the new component and we alert other existing components of the new one
		BaseComponent& component = *m_components[index].get();
		component.SetEntity(this);

		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
		{
			if (i != index)
				m_components[i]->OnComponentAttached(component);
		}

		// If we are currently disabled, inform the component
		if (!m_enabled)
			component.OnEntityDisabled();

		return component;
	}

	/*!
	* \brief Clones the entity
	* \return The clone newly created
	*
	* \remark The close is enable by default, even if the original is disabled
	* \remark Produces a NazaraAssert if the entity is not valid
	*/
	const EntityHandle& Entity::Clone() const
	{
		NazaraAssert(IsValid(), "Invalid entity");

		return m_world->CloneEntity(m_id);
	}

	/*!
	* \brief Detaches a component from the entity
	* \return An owning pointer to the component
	*
	* Instantly detaches a component from the entity and returns it, allowing to attach it to another entity
	*
	* \remark Unlike RemoveComponent, this function instantly removes the component
	*/
	std::unique_ptr<BaseComponent> Entity::DropComponent(ComponentIndex index)
	{
		if (!HasComponent(index))
			return nullptr;

		// We get the component and we alert existing components of the deleted one
		std::unique_ptr<BaseComponent> component = std::move(m_components[index]);
		m_components[index].reset();

		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
		{
			if (i != index)
				m_components[i]->OnComponentDetached(*component);
		}
		m_componentBits.Reset(index);
		m_removedComponentBits.UnboundedReset(index);

		component->SetEntity(nullptr);

		return component;
	}

	/*!
	* \brief Enables the entity
	*
	* \param enable Should the entity be enabled
	*/
	void Entity::Enable(bool enable)
	{
		if (m_enabled != enable)
		{
			m_enabled = enable;
			if (m_enabled)
			{
				for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
					m_components[i]->OnEntityEnabled();

				OnEntityEnabled(this);
			}
			else
			{
				for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
					m_components[i]->OnEntityDisabled();

				OnEntityDisabled(this);
			}

			Invalidate();
		}
	}

	/*!
	* \brief Kills the entity
	*/
	void Entity::Kill()
	{
		m_world->KillEntity(this);
	}

	/*!
	* \brief Invalidates the entity
	*/
	void Entity::Invalidate()
	{
		// We alert everyone that we have been updated
		m_world->Invalidate(m_id);
	}


	/*!
	* \brief Checks if the entity has been killed this update
	* \return True if the entity is currently dying and will be dead at next world refresh
	*/
	bool Entity::IsDying() const
	{
		return m_world->IsEntityDying(m_id);
	}

	/*!
	* \brief Creates the entity
	*/

	void Entity::Create()
	{
		m_enabled = true;
		m_valid = true;
	}

	/*!
	* \brief Destroys the entity
	*/

	void Entity::Destroy()
	{
		OnEntityDestruction(this);
		OnEntityDestruction.Clear();

		// We prepare components for entity destruction (some components needs this to handle some final callbacks while the entity is still valid)
		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			m_components[i]->OnEntityDestruction();

		// Detach components while they're still attached to systems
		for (std::size_t i = m_componentBits.FindFirst(); i != m_componentBits.npos; i = m_componentBits.FindNext(i))
			m_components[i]->SetEntity(nullptr);

		// We alert each system
		for (std::size_t index = m_systemBits.FindFirst(); index != m_systemBits.npos; index = m_systemBits.FindNext(index))
		{
			auto sysIndex = static_cast<Ndk::SystemIndex>(index);

			if (m_world->HasSystem(sysIndex))
			{
				BaseSystem& system = m_world->GetSystem(sysIndex);
				system.RemoveEntity(this);
			}
		}
		m_systemBits.Clear();

		// Destroy components
		m_components.clear();
		m_componentBits.Reset();

		// Free every handle
		UnregisterAllHandles();

		// Remove from every list
		for (EntityList* list : m_containedInLists)
			list->NotifyEntityDestruction(this);

		m_containedInLists.clear();

		m_valid = false;
	}
}
