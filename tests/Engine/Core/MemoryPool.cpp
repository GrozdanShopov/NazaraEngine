#include <Nazara/Core/MemoryPool.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector2.hpp>

SCENARIO("MemoryPool", "[CORE][MEMORYPOOL]")
{
	GIVEN("A MemoryPool to contain one Nz::Vector2<int>")
	{
		Nz::MemoryPool memoryPool(sizeof(Nz::Vector2<int>), 1, false);

		WHEN("We construct a Nz::Vector2<int>")
		{
			Nz::Vector2<int>* vector2 = memoryPool.New<Nz::Vector2<int>>(1, 2);

			THEN("Memory is available")
			{
				vector2->x = 3;
				REQUIRE(*vector2 == Nz::Vector2<int>(3, 2));
			}

			THEN("We can destroy the vector2")
			{
				memoryPool.Delete(vector2);
			}
		}
	}
}
