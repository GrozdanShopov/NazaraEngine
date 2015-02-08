// Copyright (C) 2015 J�r�me Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline World::World() :
	m_nextIndex(0)
	{
	}

	inline World::EntityList World::CreateEntities(unsigned int count)
	{
		EntityList list;
		for (unsigned int i = 0; i < count; ++i)
			list.push_back(CreateEntity());

		return list;
	}

	inline void World::KillEntities(EntityList& list)
	{
		m_killedEntities.reserve(m_killedEntities.size() + list.size());
		for (Entity& entity : list)
			KillEntity(entity);
	}

	inline bool World::IsEntityValid(const Entity& entity) const
	{
		///DOC: Cette m�thode v�rifie �galement l'appartenance de l'entit� au monde (et est donc plus s�re)
		return entity.GetWorld() == this && IsEntityIdValid(entity.GetId());
	}

	inline bool World::IsEntityIdValid(Entity::Id id) const
	{
		///DOC: Il est possible que si l'identifiant vienne d'un autre monde, il soit consid�r� valide
		///     alors qu'aucune entit� de ce monde-ci ne l'utilise (encore)

		return m_entitiesCounter[id.part.index] == id.part.counter;
	}
}
