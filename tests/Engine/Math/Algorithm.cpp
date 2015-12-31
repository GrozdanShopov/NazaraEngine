#include <Nazara/Math/Algorithm.hpp>
#include <Catch/catch.hpp>

TEST_CASE("Approach", "[MATH][ALGORITHM]")
{
	SECTION("Approach 8 with 5 by 2")
	{
		REQUIRE(Nz::Approach(5, 8, 2) == 7);
	}

	SECTION("Approach 5 with 8 by 2")
	{
		REQUIRE(Nz::Approach(8, 5, 2) == 6);
	}

	SECTION("Approach 8 with 8 by 2")
	{
		REQUIRE(Nz::Approach(8, 8, 2) == 8);
	}
}

TEST_CASE("Clamp", "[ALGORITHM]")
{
	SECTION("Clamp 8 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(8, 5, 10) == 8);
	}

	SECTION("Clamp 4 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(4, 5, 10) == 5);
	}

	SECTION("Clamp 12 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(12, 5, 10) == 10);
	}
}

TEST_CASE("CountBits", "[ALGORITHM]")
{
	SECTION("Number 10 has 2 bits set to 1")
	{
		REQUIRE(Nz::CountBits(10) == 2);
	}

	SECTION("Number 0 has 0 bit set to 1")
	{
		REQUIRE(Nz::CountBits(0) == 0);
	}
}

TEST_CASE("DegreeToRadian", "[ALGORITHM]")
{
	SECTION("Convert 45.f degree to radian")
	{
		REQUIRE(Nz::DegreeToRadian(45.f) == Approx(M_PI / 4));
	}
}

TEST_CASE("GetNearestPowerOfTwo", "[ALGORITHM]")
{
	SECTION("Nearest power of two of 0 = 1")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(0) == 1);
	}

	SECTION("Nearest power of two of 16 = 16")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(16) == 16);
	}

	SECTION("Nearest power of two of 17 = 32")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(17) == 32);
	}
}

TEST_CASE("GetNumberLength", "[ALGORITHM]")
{
	SECTION("GetNumberLength of -127 signed char")
	{
		signed char minus127 = -127;
		REQUIRE(Nz::GetNumberLength(minus127) == 4);
	}

	SECTION("GetNumberLength of 255 unsigned char")
	{
		unsigned char plus255 = 255;
		REQUIRE(Nz::GetNumberLength(plus255) == 3);
	}

	SECTION("GetNumberLength of -1270 signed int")
	{
		signed int minus1270 = -1270;
		REQUIRE(Nz::GetNumberLength(minus1270) == 5);
	}

	SECTION("GetNumberLength of 2550 unsigned int")
	{
		unsigned int plus2550 = 2550;
		REQUIRE(Nz::GetNumberLength(plus2550) == 4);
	}

	SECTION("GetNumberLength of -1270 signed long long")
	{
		signed long long minus12700 = -12700;
		REQUIRE(Nz::GetNumberLength(minus12700) == 6);
	}

	SECTION("GetNumberLength of 2550 unsigned long long")
	{
		unsigned long long plus25500 = 25500;
		REQUIRE(Nz::GetNumberLength(plus25500) == 5);
	}

	SECTION("GetNumberLength of -2.456f float")
	{
		float minus2P456 = -2.456f;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}

	SECTION("GetNumberLength of -2.456 double")
	{
		double minus2P456 = -2.456;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}

	SECTION("GetNumberLength of -2.456 long double")
	{
		long double minus2P456 = -2.456L;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}
}

TEST_CASE("IntegralLog2", "[ALGORITHM]")
{
	SECTION("According to implementation, log in base 2 of 0 = 0")
	{
		REQUIRE(Nz::IntegralLog2(0) == 0);
	}

	SECTION("Log in base 2 of 1 = 0")
	{
		REQUIRE(Nz::IntegralLog2(1) == 0);
	}

	SECTION("Log in base 2 of 4 = 2")
	{
		REQUIRE(Nz::IntegralLog2(4) == 2);
	}

	SECTION("Log in base 2 of 5 = 2")
	{
		REQUIRE(Nz::IntegralLog2(5) == 2);
	}
}

TEST_CASE("IntegralLog2Pot", "[ALGORITHM]")
{
	SECTION("According to implementation, log in base 2 of 0 = 0")
	{
		REQUIRE(Nz::IntegralLog2Pot(0) == 0);
	}

	SECTION("Log in base 2 of 1 = 0")
	{
		REQUIRE(Nz::IntegralLog2Pot(1) == 0);
	}

	SECTION("Log in base 2 of 4 = 2")
	{
		REQUIRE(Nz::IntegralLog2Pot(4) == 2);
	}
}

TEST_CASE("IntegralPow", "[ALGORITHM]")
{
	SECTION("2 to power 4")
	{
		REQUIRE(Nz::IntegralPow(2, 4) == 16);
	}
}

TEST_CASE("Lerp", "[ALGORITHM]")
{
	SECTION("Lerp 2 to 6 with 0.5")
	{
		REQUIRE(Nz::Lerp(2, 6, 0.5) == 4);
	}
}

TEST_CASE("MultiplyAdd", "[ALGORITHM]")
{
	SECTION("2 * 3 + 1")
	{
		REQUIRE(Nz::MultiplyAdd(2, 3, 1) == 7);
	}
}

TEST_CASE("NumberEquals", "[ALGORITHM]")
{
	SECTION("2.35 and 2.351 should be the same at 0.01")
	{
		CHECK(Nz::NumberEquals(2.35, 2.35, 0.01));
	}

	SECTION("3 and 4 unsigned should be the same at 1")
	{
		CHECK(Nz::NumberEquals(3U, 4U, 1U));
	}
}

TEST_CASE("NumberToString", "[ALGORITHM]")
{
	SECTION("235 to string")
	{
		REQUIRE(Nz::NumberToString(235) == "235");
	}

	SECTION("-235 to string")
	{
		REQUIRE(Nz::NumberToString(-235) == "-235");
	}

	SECTION("16 in base 16 to string")
	{
		REQUIRE(Nz::NumberToString(16, 16) == "10");
	}
}

TEST_CASE("RadianToDegree", "[ALGORITHM]")
{
	SECTION("PI / 4 to degree")
	{
		REQUIRE(Nz::RadianToDegree(M_PI / 4) == Approx(45.f));
	}
}

TEST_CASE("StringToNumber", "[ALGORITHM]")
{
	SECTION("235 in string")
	{
		REQUIRE(Nz::StringToNumber("235") == 235);
	}

	SECTION("-235 in string")
	{
		REQUIRE(Nz::StringToNumber("-235") == -235);
	}

	SECTION("16 in base 16 in string")
	{
		REQUIRE(Nz::StringToNumber("10", 16) == 16);
	}
}
