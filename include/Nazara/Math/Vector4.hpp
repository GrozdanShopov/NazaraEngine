// Copyright (C) 2012 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR4_HPP
#define NAZARA_VECTOR4_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T> class NzVector4
{
	public:
		NzVector4();
		NzVector4(T X, T Y, T Z, T W = 1.0);
		explicit NzVector4(T scale);
		NzVector4(T vec[4]);
		NzVector4(const NzVector3<T>& vec, T W = 1.0);
		template<typename U> explicit NzVector4(const NzVector4<U>& vec);
		NzVector4(const NzVector4& vec) = default;
		~NzVector4() = default;

		T AbsDotProduct(const NzVector4& vec) const;

		T DotProduct(const NzVector4& vec) const;

		void MakeCeil(const NzVector4& vec);
		void MakeFloor(const NzVector4& vec);
		void MakeUnitX();
		void MakeUnitY();
		void MakeUnitZ();
		void MakeZero();

		void Normalize();

		void Set(T X, T Y, T Z, T W = 1.0);
		void Set(T scale);
		void Set(T vec[4]);
		void Set(const NzVector3<T>& vec, T W = 1.0);
		template<typename U> void Set(const NzVector4<U>& vec);

		NzString ToString() const;

		operator NzString() const;

		operator T*();
		operator const T*() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		const NzVector4& operator+() const;
		NzVector4 operator-() const;

		NzVector4 operator+(const NzVector4& vec) const;
		NzVector4 operator-(const NzVector4& vec) const;
		NzVector4 operator*(const NzVector4& vec) const;
		NzVector4 operator*(T scale) const;
		NzVector4 operator/(const NzVector4& vec) const;
		NzVector4 operator/(T scale) const;

		NzVector4& operator+=(const NzVector4& vec);
		NzVector4& operator-=(const NzVector4& vec);
		NzVector4& operator*=(const NzVector4& vec);
		NzVector4& operator*=(T scale);
		NzVector4& operator/=(const NzVector4& vec);
		NzVector4& operator/=(T scale);

		bool operator==(const NzVector4& vec) const;
		bool operator!=(const NzVector4& vec) const;
		bool operator<(const NzVector4& vec) const;
		bool operator<=(const NzVector4& vec) const;
		bool operator>(const NzVector4& vec) const;
		bool operator>=(const NzVector4& vec) const;

		static NzVector4 UnitX();
		static NzVector4 UnitY();
		static NzVector4 UnitZ();
		static NzVector4 Zero();

		T x, y, z, w;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzVector4<T>& vec);

template<typename T> NzVector4<T> operator*(T scale, const NzVector4<T>& vec);
template<typename T> NzVector4<T> operator/(T scale, const NzVector4<T>& vec);

typedef NzVector4<double> NzVector4d;
typedef NzVector4<float> NzVector4f;
typedef NzVector4<int> NzVector4i;

#include <Nazara/Math/Vector4.inl>

#endif // NAZARA_VECTOR4_HPP
