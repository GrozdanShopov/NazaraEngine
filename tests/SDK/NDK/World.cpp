#include <NDK/World.hpp>
#include <NDK/Component.hpp>
#include <Catch/catch.hpp>

namespace
{
	class UpdatableComponent : public Ndk::Component<UpdatableComponent>
	{
		public:
			bool IsUpdated()
			{
				return m_updated;
			}

			void SetUpdated()
			{
				m_updated = true;
			}

			static Ndk::ComponentIndex componentIndex;

		private:
			bool m_updated = false;
	};

	Ndk::ComponentIndex UpdatableComponent::componentIndex;

	class UpdateSystem : public Ndk::System<UpdateSystem>
	{
		public:
			UpdateSystem()
			{
				Requires<UpdatableComponent>();
			}

			~UpdateSystem() = default;

			static Ndk::SystemIndex systemIndex;

		private:
			void OnUpdate(float /*elapsedTime*/) override
			{
				for (const Ndk::EntityHandle& entity : GetEntities())
				{
					UpdatableComponent& updatable = entity->GetComponent<UpdatableComponent>();
					updatable.SetUpdated();
				}
			}
	};

	Ndk::SystemIndex UpdateSystem::systemIndex;
}

SCENARIO("World", "[NDK][WORLD]")
{
	GIVEN("A brave new world and the update system")
	{
		Ndk::World world(false);
		Ndk::BaseSystem& system = world.AddSystem<UpdateSystem>();

		WHEN("We had a new entity with an updatable component and a system")
		{
			const Ndk::EntityHandle& entity = world.CreateEntity();
			UpdatableComponent& component = entity->AddComponent<UpdatableComponent>();

			THEN("We can get our entity and our system")
			{
				const Ndk::EntityHandle& fetchedEntity = world.GetEntity(entity->GetId());
				REQUIRE(fetchedEntity->GetWorld() == &world);
			}

			THEN("We can clone it")
			{
				const Ndk::EntityHandle& clone = world.CloneEntity(entity->GetId());
				REQUIRE(world.IsEntityValid(clone));
			}
		}

		AND_WHEN("We update our world with our entity")
		{
			REQUIRE(&world.GetSystem(UpdateSystem::systemIndex) == &world.GetSystem<UpdateSystem>());
			Ndk::EntityHandle entity = world.CreateEntity();
			UpdatableComponent& component = entity->AddComponent<UpdatableComponent>();

			THEN("Our entity component must be updated")
			{
				world.Update(1.f);

				REQUIRE(component.IsUpdated());
			}

			THEN("We kill our entity")
			{
				REQUIRE(entity->IsValid());

				world.KillEntity(entity);
				world.Update(1.f);

				REQUIRE(!world.IsEntityValid(entity));
			}
		}
	}

	GIVEN("A newly created entity")
	{
		Ndk::World world(false);
		Ndk::EntityHandle entity = world.CreateEntity();

		REQUIRE(entity.IsValid());
		REQUIRE(entity->IsValid());
		CHECK_FALSE(entity->IsDying());

		WHEN("We kill it")
		{
			entity->Kill();

			CHECK(entity.IsValid());
			CHECK(entity->IsValid());
			CHECK(entity->IsDying());

			THEN("We refresh the world")
			{
				world.Refresh();

				CHECK_FALSE(entity.IsValid());
			}
		}
	}

	GIVEN("An empty world")
	{
		Ndk::World world(false);

		WHEN("We create two entities")
		{
			Ndk::EntityHandle a = world.CreateEntity();
			REQUIRE(a->GetId() == 0);
			Ndk::EntityHandle b = world.CreateEntity();
			REQUIRE(b->GetId() == 1);

			b->OnEntityDestruction.Connect([a](Ndk::Entity*)
			{
				REQUIRE(a.IsValid());
				a->Kill();
			});

			THEN("We kill the second entity which will kill the first one")
			{
				b->Kill();
				world.Refresh();

				AND_THEN("Both entities should be dead next refresh")
				{
					world.Refresh();

					REQUIRE_FALSE(a.IsValid());
					REQUIRE_FALSE(b.IsValid());
				}
			}
		}
	}
}
