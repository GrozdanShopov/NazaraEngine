// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASHDIGEST_HPP
#define NAZARA_HASHDIGEST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <ostream>

class NAZARA_API NzHashDigest
{
	public:
		NzHashDigest();
		NzHashDigest(const NzString& hashName, const nzUInt8* digest, unsigned int length);
		NzHashDigest(const NzHashDigest& rhs);
		NzHashDigest(NzHashDigest&& rhs);
		~NzHashDigest();

		bool IsValid() const;

		const nzUInt8* GetDigest() const;
		unsigned int GetDigestLength() const;
		NzString GetHashName() const;

		NzString ToHex() const;

		nzUInt8 operator[](unsigned short pos) const;

		NzHashDigest& operator=(const NzHashDigest& rhs);
		NzHashDigest& operator=(NzHashDigest&& rhs);

		bool operator==(const NzHashDigest& rhs) const;
		bool operator!=(const NzHashDigest& rhs) const;
		bool operator<(const NzHashDigest& rhs) const;
		bool operator<=(const NzHashDigest& rhs) const;
		bool operator>(const NzHashDigest& rhs) const;
		bool operator>=(const NzHashDigest& rhs) const;

		NAZARA_API friend std::ostream& operator<<(std::ostream& out, const NzHashDigest& string);

	private:
		NzString m_hashName;
		nzUInt8* m_digest;
		unsigned short m_digestLength;
};

#endif // NAZARA_HASHDIGEST_HPP
