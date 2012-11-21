// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CUBE_HPP
#define NAZARA_CUBE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzCube
{
	public:
		NzCube() = default;
		NzCube(T X, T Y, T Z, T Width, T Height, T Depth);
		NzCube(const T cube[6]);
		NzCube(const NzRect<T>& rect);
		NzCube(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzCube(const NzCube<U>& cube);
		NzCube(const NzCube& cube) = default;
		~NzCube() = default;

		bool Contains(T X, T Y, T Z) const;
		bool Contains(const NzVector3<T>& point) const;
		bool Contains(const NzCube& cube) const;

		NzCube& ExtendTo(const NzVector3<T>& point);
		NzCube& ExtendTo(const NzCube& cube);

		NzVector3<T> GetCenter() const;
		NzVector3<T> GetPosition() const;
		NzVector3<T> GetSize() const;

		bool Intersect(const NzCube& cube, NzCube* intersection = nullptr) const;

		bool IsValid() const;

		NzCube& MakeZero();

		NzCube& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzCube& Set(const T cube[6]);
		NzCube& Set(const NzRect<T>& rect);
		NzCube& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzCube& Set(const NzCube<U>& cube);

		NzString ToString() const;

		operator NzString() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		NzCube operator*(T scalar) const;

		NzCube& operator*=(T scalar);

		bool operator==(const NzCube& cube) const;
		bool operator!=(const NzCube& cube) const;

		static NzCube Lerp(const NzCube& from, const NzCube& to, T interpolation);
		static NzCube Zero();

		T x, y, z, width, height, depth;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzCube<T>& vec);

typedef NzCube<double> NzCubed;
typedef NzCube<float> NzCubef;
typedef NzCube<int> NzCubei;
typedef NzCube<unsigned int> NzCubeui;

#include <Nazara/Math/Cube.inl>

#endif // NAZARA_CUBE_HPP
