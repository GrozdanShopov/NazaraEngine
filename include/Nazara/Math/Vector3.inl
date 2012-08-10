// Copyright (C) 2012 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzVector3<T>::NzVector3()
{
}

template<typename T>
NzVector3<T>::NzVector3(T X, T Y, T Z)
{
	Set(X, Y, Z);
}

template<typename T>
NzVector3<T>::NzVector3(T scale)
{
	Set(scale);
}

template<typename T>
NzVector3<T>::NzVector3(T vec[3])
{
	Set(vec);
}

template<typename T>
NzVector3<T>::NzVector3(const NzVector2<T>& vec, T Z)
{
	Set(vec, Z);
}

template<typename T>
template<typename U>
NzVector3<T>::NzVector3(const NzVector3<U>& vec)
{
	Set(vec);
}

template<typename T>
T NzVector3<T>::AbsDotProduct(const NzVector3& vec) const
{
	return std::fabs(x * vec.x) + std::fabs(y * vec.y) + std::fabs(z * vec.z);
}

template<>
inline int NzVector3<int>::AbsDotProduct(const NzVector3<int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z);
}

template<>
inline unsigned int NzVector3<unsigned int>::AbsDotProduct(const NzVector3<unsigned int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::CrossProduct(const NzVector3& vec) const
{
	return NzVector3(y * vec.z - z * vec.y, z * vec.x - x * vec.y, x * vec.y - y * vec.x);
}

template<typename T>
T NzVector3<T>::Distance(const NzVector3& vec) const
{
	return std::sqrt(SquaredDistance(vec));
}

template<typename T>
float NzVector3<T>::Distancef(const NzVector3& vec) const
{
	return std::sqrt(static_cast<float>(SquaredDistance()));
}

template<typename T>
T NzVector3<T>::DotProduct(const NzVector3& vec) const
{
	return x*vec.x + y*vec.y + z*vec.z;
}

template<typename T>
NzVector3<T> NzVector3<T>::GetNormal() const
{
	NzVector3 vec(*this);
	vec.Normalize();

	return vec;
}

template<typename T>
void NzVector3<T>::MakeCeil(const NzVector3& vec)
{
	if (vec.x > x)
		x = vec.x;

	if (vec.y > y)
		y = vec.y;

    if (vec.z > z)
        z = vec.z;
}

template<typename T>
void NzVector3<T>::MakeFloor(const NzVector3& vec)
{
	if (vec.x < x)
		x = vec.x;

	if (vec.y < y)
		y = vec.y;

    if (vec.z < z)
        z = vec.z;
}

template<typename T>
void NzVector3<T>::MakeUnitX()
{
	Set(F(1.0), F(0.0), F(0.0));
}

template<typename T>
void NzVector3<T>::MakeUnitY()
{
	Set(F(0.0), F(1.0), F(0.0));
}

template<typename T>
void NzVector3<T>::MakeUnitZ()
{
	Set(F(0.0), F(0.0), F(1.0));
}

template<typename T>
void NzVector3<T>::MakeZero()
{
	Set(F(0.0), F(0.0), F(0.0));
}

template<typename T>
T NzVector3<T>::Length() const
{
	return std::sqrt(SquaredLength());
}

template<typename T>
float NzVector3<T>::Lengthf() const
{
	return std::sqrt(static_cast<float>(SquaredLength()));
}

template<typename T>
void NzVector3<T>::Normalize()
{
	T squaredLength = SquaredLength();

	if (squaredLength-F(1.0) > std::numeric_limits<T>::epsilon())
	{
		T length = std::sqrt(squaredLength);

		x /= length;
		y /= length;
		z /= length;
	}
}

template<typename T>
void NzVector3<T>::Set(T X, T Y, T Z)
{
	x = X;
	y = Y;
	z = Z;
}

template<typename T>
void NzVector3<T>::Set(T scale)
{
	x = scale;
	y = scale;
	z = scale;
}

template<typename T>
void NzVector3<T>::Set(T vec[3])
{
	std::memcpy(&x, vec, 3*sizeof(T));
}

template<typename T>
void NzVector3<T>::Set(const NzVector2<T>& vec, T Z)
{
	x = vec.x;
	y = vec.y;
	z = Z;
}

template<typename T>
template<typename U>
void NzVector3<T>::Set(const NzVector3<U>& vec)
{
	x = F(vec.x);
	y = F(vec.y);
	z = F(vec.z);
}

template<typename T>
T NzVector3<T>::SquaredDistance(const NzVector3& vec) const
{
	return operator-(vec).SquaredLength();
}

template<typename T>
T NzVector3<T>::SquaredLength() const
{
	return x*x + y*y + z*z;
}

template<typename T>
NzString NzVector3<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Vector3(" << x << ", " << y << ", " << z <<')';
}

template<typename T>
NzVector3<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
NzVector3<T>::operator T*()
{
	return &x;
}

template<typename T>
NzVector3<T>::operator const T*() const
{
	return &x;
}

template<typename T>
T& NzVector3<T>::operator[](unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i >= 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
T NzVector3<T>::operator[](unsigned int i) const
{
	#if NAZARA_MATH_SAFE
	if (i >= 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
const NzVector3<T>& NzVector3<T>::operator+() const
{
	return *this;
}

template<typename T>
NzVector3<T> NzVector3<T>::operator-() const
{
	return NzVector3(-x, -y, -z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator+(const NzVector3& vec) const
{
	return NzVector3(x + vec.x, y + vec.y, z + vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator-(const NzVector3& vec) const
{
	return NzVector3(x - vec.x, y - vec.y, z - vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator*(const NzVector3& vec) const
{
	return NzVector3(x * vec.x, y * vec.y, z * vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator*(T scale) const
{
	return NzVector3(x * scale, y * scale, z * scale);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator/(const NzVector3& vec) const
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}
	#endif

	return NzVector3(x / vec.x, y / vec.y, z / vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator/(T scale) const
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(scale, F(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}
	#endif

	return NzVector3(x / scale, y / scale, z / scale);
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator+=(const NzVector3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator-=(const NzVector3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator*=(const NzVector3& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator/=(const NzVector3& vec)
{
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= vec.x;
	y /= vec.y;
	z /= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator/=(T scale)
{
	if (NzNumberEquals(scale, F(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= scale;
	y /= scale;
	z /= scale;

	return *this;
}

template<typename T>
bool NzVector3<T>::operator==(const NzVector3& vec) const
{
	return NzNumberEquals(x, vec.x) &&
		   NzNumberEquals(y, vec.y) &&
		   NzNumberEquals(z, vec.z);
}

template<typename T>
bool NzVector3<T>::operator!=(const NzVector3& vec) const
{
	return !operator==(vec);
}

template<typename T>
bool NzVector3<T>::operator<(const NzVector3& vec) const
{
	return x < vec.x && y < vec.y && z < vec.z;
}

template<typename T>
bool NzVector3<T>::operator<=(const NzVector3& vec) const
{
	return operator<(vec) || operator==(vec);
}

template<typename T>
bool NzVector3<T>::operator>(const NzVector3& vec) const
{
	return !operator<=(vec);
}

template<typename T>
bool NzVector3<T>::operator>=(const NzVector3& vec) const
{
	return !operator<(vec);
}

template<typename T>
NzVector3<T> NzVector3<T>::UnitX()
{
	NzVector3 vector;
	vector.MakeUnitX();

	return vector;
}

template<typename T>
NzVector3<T> NzVector3<T>::UnitY()
{
	NzVector3 vector;
	vector.MakeUnitY();

	return vector;
}

template<typename T>
NzVector3<T> NzVector3<T>::UnitZ()
{
	NzVector3 vector;
	vector.MakeUnitZ();

	return vector;
}

template<typename T>
NzVector3<T> NzVector3<T>::Zero()
{
	NzVector3 vector;
	vector.MakeZero();

	return vector;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzVector3<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
NzVector3<T> operator*(T scale, const NzVector3<T>& vec)
{
	return NzVector3<T>(scale * vec.x, scale * vec.y, scale * vec.z);
}

template<typename T>
NzVector3<T> operator/(T scale, const NzVector3<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}
	#endif

	return NzVector3<T>(scale / vec.x, scale / vec.y, scale / vec.z);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
