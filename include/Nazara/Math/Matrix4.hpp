// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATRIX4_HPP
#define NAZARA_MATRIX4_HPP

#include <Nazara/Core/String.hpp>

#define NAZARA_CLASS_MATRIX4
#include <Nazara/Math/ThreadSafety.hpp>

template<typename T> class NzEulerAngles;
template<typename T> class NzQuaternion;
template<typename T> class NzVector2;
template<typename T> class NzVector3;
template<typename T> class NzVector4;

template<typename T> class NzMatrix4
{
	public:
		NzMatrix4();
		NzMatrix4(T r11, T r12, T r13, T r14,
				  T r21, T r22, T r23, T r24,
				  T r31, T r32, T r33, T r34,
				  T r41, T r42, T r43, T r44);
		NzMatrix4(T matrix[16]);
		//NzMatrix4(const NzMatrix3<T>& matrix);
		template<typename U> explicit NzMatrix4(const NzMatrix4<U>& matrix);
		NzMatrix4(const NzMatrix4& matrix);
		NzMatrix4(NzMatrix4&& matrix);
		~NzMatrix4();

		T GetDeterminant() const;
		NzMatrix4 GetInverse() const;
		//NzMatrix3 GetRotationMatrix() const;
		NzVector3<T> GetScale() const;
		NzVector3<T> GetTranslation() const;
		NzMatrix4 GetTransposed() const;

		bool HasNegativeScale() const;
		bool HasScale() const;

		void Set(T r11, T r12, T r13, T r14,
				 T r21, T r22, T r23, T r24,
				 T r31, T r32, T r33, T r34,
				 T r41, T r42, T r43, T r44);
		void Set(T matrix[16]);
		//NzMatrix4(const NzMatrix3<T>& matrix);
		void Set(const NzMatrix4& matrix);
		void Set(NzMatrix4&& matrix);
		template<typename U> void Set(const NzMatrix4<U>& matrix);
		void SetIdentity();
		void SetLookAt(const NzVector3<T>& eye, const NzVector3<T>& center, const NzVector3<T>& up);
		void SetOrtho(T left, T top, T width, T height, T zNear = -1.0, T zFar = 1.0);
		void SetPerspective(T angle, T ratio, T zNear, T zFar);
		void SetRotation(const NzQuaternion<T>& rotation);
		void SetScale(const NzVector3<T>& scale);
		void SetTranslation(const NzVector3<T>& translation);
		void SetZero();

		NzString ToString() const;

		NzVector2<T> Transform(const NzVector2<T>& vector, T z = 0.0, T w = 1.0) const;
		NzVector3<T> Transform(const NzVector3<T>& vector, T w = 1.0) const;
		NzVector4<T> Transform(const NzVector4<T>& vector) const;

		NzMatrix4& Transpose();

		operator T*();
		operator const T*() const;

		T& operator()(unsigned int x, unsigned int y);
		const T& operator()(unsigned int x, unsigned int y) const;

		NzMatrix4& operator=(const NzMatrix4& matrix);
		NzMatrix4& operator=(NzMatrix4&& matrix);

		NzMatrix4 operator*(const NzMatrix4& matrix) const;
		NzVector2<T> operator*(const NzVector2<T>& vector) const;
		NzVector3<T> operator*(const NzVector3<T>& vector) const;
		NzVector4<T> operator*(const NzVector4<T>& vector) const;
		NzMatrix4 operator*(T scalar) const;

		NzMatrix4& operator*=(const NzMatrix4& matrix);
		NzMatrix4& operator*=(T scalar);

		static NzMatrix4 LookAt(const NzVector3<T>& eye, const NzVector3<T>& center, const NzVector3<T>& up);
		static NzMatrix4 Ortho(T left, T top, T width, T height, T zNear = -1.0, T zFar = 1.0);
		static NzMatrix4 Perspective(T angle, T ratio, T zNear, T zFar);
		static NzMatrix4 Rotate(const NzQuaternion<T>& rotation);
		static NzMatrix4 Scale(const NzVector3<T>& scale);
		static NzMatrix4 Translate(const NzVector3<T>& translation);

		struct SharedMatrix
		{
			T m11, m12, m13, m14;
			T m21, m22, m23, m24;
			T m31, m32, m33, m34;
			T m41, m42, m43, m44;

			unsigned short refCount = 1;
			NazaraMutex(mutex)
		};

	private:
		void EnsureOwnership();
		void ReleaseMatrix();

		SharedMatrix* m_sharedMatrix;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzMatrix4<T>& matrix);

typedef NzMatrix4<double> NzMatrix4d;
typedef NzMatrix4<float> NzMatrix4f;

#undef NAZARA_CLASS_MATRIX4

#include <Nazara/Math/Matrix4.inl>

#endif // NAZARA_MATRIX4_HPP
