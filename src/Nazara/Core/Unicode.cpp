// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Core/Debug.hpp>

namespace NzUnicode
{
	#if NAZARA_CORE_INCLUDE_UNICODEDATA
	struct Character
	{
		nzUInt16 category;	// Le type du caract�re
		nzUInt8	 direction;	// Le sens de lecure du caract�re
		nzUInt32 lowerCase;	// Le caract�re correspondant en minuscule
		nzUInt32 titleCase;	// Le caract�re correspondant en titre
		nzUInt32 upperCase;	// Le caract�re correspondant en majuscule
	};

	#include <Nazara/Core/UnicodeData.hpp>

	#else // Impl�mentation bidon

	Category GetCategory(char32_t character)
	{
		NazaraUnused(character);

		return Category_NoCategory;
	}

	Direction GetDirection(char32_t character)
	{
		NazaraUnused(character);

		return Direction_Boundary_Neutral;
	}

	char32_t GetLowercase(char32_t character)
	{
		return character;
	}

	char32_t GetTitlecase(char32_t character)
	{
		return character;
	}

	char32_t GetUppercase(char32_t character)
	{
		return character;
	}
	#endif
}
