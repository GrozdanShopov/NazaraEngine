#include <Nazara/Core/String.hpp>
#include <catch.hpp>

SCENARIO("String", "[CORE][STRING]")
{
	GIVEN("One string 'a'")
	{
		NzString aDefaultString(1, 'a');

		WHEN("We add information")
		{
			aDefaultString.Append("Default");
			aDefaultString.Insert(aDefaultString.GetSize(), "String");

			THEN("The result should be 'aDefaultString'")
			{
				REQUIRE(aDefaultString == "aDefaultString");
				REQUIRE(aDefaultString.GetSize() == 14);
				REQUIRE(aDefaultString.GetCapacity() >= 14);
			}

			AND_WHEN("We test Contains and Find")
			{
				THEN("These results are expected")
				{
					CHECK(aDefaultString.Contains('D'));
					CHECK(aDefaultString.Contains("String", 3));
					CHECK(aDefaultString.Contains(NzString("sTRING"), 3, NzString::CaseInsensitive));
					REQUIRE(aDefaultString.FindLast('g') == aDefaultString.GetSize() - 1);
					CHECK(aDefaultString.EndsWith('G', NzString::CaseInsensitive));
					aDefaultString.Append(" ng bla");
					REQUIRE(aDefaultString.FindWord("ng") == aDefaultString.GetSize() - 6);
					//TODO REQUIRE(aDefaultString.FindWord(NzString("ng")) == aDefaultString.GetSize() - 6);
					CHECK(aDefaultString.StartsWith("aD"));
				}
			}
		}

		WHEN("We do operators")
		{
			aDefaultString[0] = 'a';
			aDefaultString += "Default";
			aDefaultString = aDefaultString + "String";

			THEN("The result should be 'aDefaultString'")
			{
				REQUIRE(aDefaultString == "aDefaultString");
				REQUIRE(aDefaultString.GetSize() == 14);
				REQUIRE(aDefaultString.GetCapacity() >= 14);
			}

			AND_WHEN("We test Count")
			{
				THEN("These results are expected")
				{
					REQUIRE(aDefaultString.Count('D') == 1);
					REQUIRE(aDefaultString.Count("t", 2) == 2);
				}
			}
		}
	}

	GIVEN("The string of number 16 in base 16")
	{
		NzString number16;

		CHECK(number16.IsEmpty());
		CHECK(number16.IsNull());

		WHEN("We assign to number 16")
		{
			number16 = NzString::Number(16, 16);

			THEN("These results are expected")
			{
				CHECK(number16.IsNumber(16));
				number16.Prepend("0x");
				REQUIRE(number16.GetSize() == 4);
				REQUIRE(number16.GetCapacity() >= 4);
				REQUIRE(number16.SubStringFrom('x', 1) == "10");
			}
		}
	}

	/* TODO
	GIVEN("One unicode string")
	{
		NzString unicodeString = NzString::Unicode(U"àéçœÂ官話");

		WHEN("We convert to other UTF")
		{
			REQUIRE(unicodeString.GetSize() == 7);
			REQUIRE(unicodeString.GetCapacity() >= 7);
			CHECK(unicodeString.Contains("官"));

			THEN("The result should be the identity")
			{
				char* utf8 = unicodeString.GetUtf8Buffer();
				NzString utf8String = NzString::Unicode(utf8);
				char16_t* utf16 = unicodeString.GetUtf16Buffer();
				NzString utf16String = NzString::Unicode(utf16);
				char32_t* utf32 = unicodeString.GetUtf32Buffer();
				NzString utf32String = NzString::Unicode(utf32);

				REQUIRE(utf8String == utf16String);
				REQUIRE(utf16String == utf32String);
			}
		}
	}*/
}

