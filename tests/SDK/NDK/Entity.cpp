#include <NazaraSDK/World.hpp>
#include <NazaraSDK/Component.hpp>
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

SCENARIO("Entity", "[NDK][ENTITY]")
{
	GIVEN("A world & an entity")
	{
		Ndk::World world(false);

		Ndk::BaseSystem& system = world.AddSystem<UpdateSystem>();
		Ndk::EntityHandle entity = world.CreateEntity();

		WHEN("We add our UpdateComponent")
		{
			UpdatableComponent& updatableComponent = entity->AddComponent<UpdatableComponent>();
			CHECK(!updatableComponent.IsUpdated());

			THEN("Update the world should update the entity's component")
			{
				world.Update(1.f);
				UpdatableComponent& updatableComponentGet = entity->GetComponent<UpdatableComponent>();
				CHECK(updatableComponentGet.IsUpdated());
			}

			THEN("Update the world should not update the entity's component if it's disabled")
			{
				entity->Enable(false);
				world.Update(1.f);
				UpdatableComponent& updatableComponentGet = entity->GetComponent<UpdatableComponent>();
				CHECK(!updatableComponentGet.IsUpdated());
			}

			THEN("We can remove its component")
			{
				entity->RemoveComponent(Ndk::GetComponentIndex<UpdatableComponent>());
				world.Update(1.f);
				CHECK(!entity->HasComponent<UpdatableComponent>());
			}
		}

		WHEN("We kill our entity")
		{
			entity->Kill();
			world.Update(1.f);

			THEN("It's no more valid")
			{
				CHECK(!world.IsEntityValid(entity));
			}
		}
	}
}