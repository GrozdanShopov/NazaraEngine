#include <Nazara/Physics2D/Collider2D.hpp>
#include <Catch/catch.hpp>

SCENARIO("Collider2D", "[PHYSICS2D][COLLIDER2D]")
{
	GIVEN("No particular elements")
	{
		WHEN("We construct a box with Rect")
		{
			Nz::Rectf aabb(5.f, 3.f, 10.f, 6.f);
			Nz::BoxCollider2D box(aabb);

			THEN("We expect those to be true")
			{
				CHECK(box.GetRect() == aabb);
				CHECK(box.GetSize() == aabb.GetLengths());
				CHECK(box.GetType() == Nz::ColliderType2D_Box);
			}
		}

		WHEN("We construct a box with Vector2D")
		{
			Nz::Vector2f vec(5.f, 3.f);
			Nz::Rectf aabb(-2.5f, -1.5f, 5.f, 3.f);
			Nz::BoxCollider2D box(vec);

			THEN("We expect those to be true")
			{
				CHECK(box.GetRect() == aabb);
				CHECK(box.GetSize() == vec);
				CHECK(box.GetType() == Nz::ColliderType2D_Box);
			}
		}

		WHEN("We construct a circle")
		{
			Nz::Vector2f position(5.f, 3.f);
			float radius = 7.f;
			Nz::CircleCollider2D circle(radius, position);

			THEN("We expect those to be true")
			{
				CHECK(circle.GetRadius() == Approx(radius));
				CHECK(circle.GetType() == Nz::ColliderType2D_Circle);
			}
		}

		WHEN("We construct a compound")
		{
			Nz::Rectf aabb(0.f, 0.f, 1.f, 1.f);
			Nz::BoxCollider2DRef box1 = Nz::BoxCollider2D::New(aabb);
			aabb.Translate(Nz::Vector2f::Unit());
			Nz::BoxCollider2DRef box2 = Nz::BoxCollider2D::New(aabb);

			std::vector<Nz::Collider2DRef> colliders;
			colliders.push_back(box1);
			colliders.push_back(box2);
			Nz::CompoundCollider2D compound(colliders);

			THEN("We expect those to be true")
			{
				CHECK(compound.GetType() == Nz::ColliderType2D_Compound);
			}
		}

		WHEN("We construct a convex")
		{
			std::vector<Nz::Vector2f> vertices;
			vertices.push_back(Nz::Vector2f(0.f, 0.f));
			vertices.push_back(Nz::Vector2f(0.f, 1.f));
			vertices.push_back(Nz::Vector2f(1.f, 1.f));
			vertices.push_back(Nz::Vector2f(1.f, 0.f));

			Nz::ConvexCollider2D convex(Nz::SparsePtr<const Nz::Vector2f>(vertices.data()), vertices.size());

			THEN("We expect those to be true")
			{
				CHECK(convex.GetType() == Nz::ColliderType2D_Convex);
			}
		}

		WHEN("We construct a null")
		{
			Nz::NullCollider2D null;

			THEN("We expect those to be true")
			{
				CHECK(null.GetType() == Nz::ColliderType2D_Null);
			}
		}

		WHEN("We construct a segment")
		{
			Nz::Vector2f firstPoint(2.f, 1.f);
			Nz::Vector2f secondPoint(-4.f, -3.f);
			Nz::SegmentCollider2D segment(firstPoint, secondPoint);

			THEN("We expect those to be true")
			{
				CHECK(segment.GetFirstPoint() == firstPoint);
				CHECK(segment.GetLength() == firstPoint.Distance(secondPoint));
				CHECK(segment.GetSecondPoint() == secondPoint);
				CHECK(segment.GetType() == Nz::ColliderType2D_Segment);
			}
		}

		WHEN("We verify general purpose methods")
		{
			Nz::Rectf aabb(5.f, 3.f, 10.f, 6.f);
			Nz::BoxCollider2D box(aabb);

			Nz::UInt32 categoryMask = 1;
			Nz::UInt32 groupId = 2;
			Nz::UInt32 typeId = 3;
			Nz::UInt32 mask = 4;
			bool trigger = true;
			box.SetCategoryMask(categoryMask);
			box.SetCollisionGroup(groupId);
			box.SetCollisionId(typeId);
			box.SetCollisionMask(mask);
			box.SetTrigger(trigger);

			THEN("We expect those to be true")
			{
				CHECK(box.GetCategoryMask() == categoryMask);
				CHECK(box.GetCollisionGroup() == groupId);
				CHECK(box.GetCollisionId() == typeId);
				CHECK(box.GetCollisionMask() == mask);
				CHECK(box.IsTrigger() == trigger);
			}
		}
	}
}
