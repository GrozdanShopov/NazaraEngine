// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELOADER_HPP
#define NAZARA_RESOURCELOADER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/String.hpp>
#include <list>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class Stream;

	template<typename Type, typename Parameters>
	class ResourceLoader
	{
		static_assert(std::is_base_of<ResourceParameters, Parameters>::value, "ResourceParameters must be a base of Parameters");

		friend Type;

		public:
			using ExtensionGetter = bool (*)(const String& extension);
			using FileLoader = bool (*)(Type* resource, const String& filePath, const Parameters& parameters);
			using MemoryLoader = bool (*)(Type* resource, const void* data, std::size_t size, const Parameters& parameters);
			using StreamChecker = Ternary (*)(Stream& stream, const Parameters& parameters);
			using StreamLoader = bool (*)(Type* resource, Stream& stream, const Parameters& parameters);

			ResourceLoader() = delete;
			~ResourceLoader() = delete;

			static bool IsExtensionSupported(const String& extension);

			static bool LoadFromFile(Type* resource, const String& filePath, const Parameters& parameters = Parameters());
			static bool LoadFromMemory(Type* resource, const void* data, std::size_t size, const Parameters& parameters = Parameters());
			static bool LoadFromStream(Type* resource, Stream& stream, const Parameters& parameters = Parameters());

			static void RegisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader = nullptr, MemoryLoader memoryLoader = nullptr);
			static void UnregisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader = nullptr, MemoryLoader memoryLoader = nullptr);

		private:
			using Loader = std::tuple<ExtensionGetter, StreamChecker, StreamLoader, FileLoader, MemoryLoader>;
			using LoaderList = std::list<Loader>;
	};
}

#include <Nazara/Core/ResourceLoader.inl>

#endif // NAZARA_RESOURCELOADER_HPP
