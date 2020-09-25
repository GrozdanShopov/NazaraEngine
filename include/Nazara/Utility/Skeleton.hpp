// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETON_HPP
#define NAZARA_SKELETON_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Config.hpp>
#include <string>

namespace Nz
{
	class Joint;
	class Skeleton;

	using SkeletonConstRef = ObjectRef<const Skeleton>;
	using SkeletonLibrary = ObjectLibrary<Skeleton>;
	using SkeletonRef = ObjectRef<Skeleton>;

	struct SkeletonImpl;

	class NAZARA_UTILITY_API Skeleton : public RefCounted
	{
		friend Joint;
		friend SkeletonLibrary;
		friend class Utility;

		public:
			Skeleton() = default;
			Skeleton(const Skeleton& skeleton);
			~Skeleton();

			bool Create(std::size_t jointCount);
			void Destroy();

			const Boxf& GetAABB() const;
			Joint* GetJoint(const std::string& jointName);
			Joint* GetJoint(std::size_t index);
			const Joint* GetJoint(const std::string& jointName) const;
			const Joint* GetJoint(std::size_t index) const;
			Joint* GetJoints();
			const Joint* GetJoints() const;
			std::size_t GetJointCount() const;
			int GetJointIndex(const std::string& jointName) const;

			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation);
			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, std::size_t* indices, std::size_t indiceCount);

			bool IsValid() const;

			Skeleton& operator=(const Skeleton& skeleton);

			template<typename... Args> static SkeletonRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnSkeletonDestroy, const Skeleton* /*skeleton*/);
			NazaraSignal(OnSkeletonJointsInvalidated, const Skeleton* /*skeleton*/);
			NazaraSignal(OnSkeletonRelease, const Skeleton* /*skeleton*/);

		private:
			void InvalidateJoints();
			void InvalidateJointMap();
			void UpdateJointMap() const;

			static bool Initialize();
			static void Uninitialize();

			SkeletonImpl* m_impl = nullptr;

			static SkeletonLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Utility/Skeleton.inl>

#endif // NAZARA_SKELETON_HPP
