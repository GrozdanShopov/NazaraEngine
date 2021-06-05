#include <NazaraSDK/System.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/VelocityComponent.hpp>
#include <NazaraSDK/World.hpp>
#include <catch2/catch.hpp>

namespace
{
	class TestSystem : public Ndk::System<TestSystem>
	{
		public:
			TestSystem()
			{
				Requires<Ndk::VelocityComponent>();
				Excludes<Ndk::NodeComponent>();
			}

			~TestSystem() = default;

			static Ndk::SystemIndex systemIndex;

		private:
			void OnUpdate(float /*elapsedTime*/) override
			{
			}
	};

	Ndk::SystemIndex TestSystem::systemIndex;
}

SCENARIO("BaseSystem", "[NDK][BASESYSTEM]")
{
	GIVEN("Our TestSystem")
	{
		Ndk::World world;

		Ndk::BaseSystem& system = world.AddSystem<TestSystem>();
		REQUIRE(&system.GetWorld() == &world);

		WHEN("We add an entity")
		{
			Ndk::EntityHandle entity = world.CreateEntity();
			entity->AddComponent<Ndk::VelocityComponent>();

			THEN("System should have it")
			{
				world.Update(1.f);
				REQUIRE(system.HasEntity(entity));
			}
		}

		WHEN("We add an entity with excluded component")
		{
			Ndk::EntityHandle entity = world.CreateEntity();
			entity->AddComponent<Ndk::VelocityComponent>();
			entity->AddComponent<Ndk::NodeComponent>();

			THEN("System should not have it")
			{
				world.Update(1.f);
				REQUIRE(!system.HasEntity(entity));
			}
		}
	}
}
