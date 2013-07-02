// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::IsExtensionSupported(const NzString& extension)
{
	for (Loader& loader : Type::s_loaders)
	{
		ExtensionGetter isExtensionSupported = std::get<0>(loader);

		if (isExtensionSupported && isExtensionSupported(extension))
			return true;
	}

	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters)
{
	#if NAZARA_CORE_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid parameters");
		return false;
	}
	#endif

	NzString path = NzFile::NormalizePath(filePath);
	NzString ext = path.SubStringFrom('.', -1, true);
	if (ext.IsEmpty())
	{
		NazaraError("Failed to get file extension from \"" + filePath + '"');
		return false;
	}

	NzFile file(path); // Ouvert seulement en cas de besoin

	bool found = false;
	for (Loader& loader : Type::s_loaders)
	{
		ExtensionGetter isExtensionSupported = std::get<0>(loader);
		if (!isExtensionSupported || !isExtensionSupported(ext))
			continue;

		StreamChecker checkFunc = std::get<1>(loader);
		StreamLoader streamLoader = std::get<2>(loader);
		FileLoader fileLoader = std::get<3>(loader);

		if (checkFunc && !file.IsOpen())
		{
			if (!file.Open(NzFile::ReadOnly))
			{
				NazaraError("Failed to load file: unable to open \"" + filePath + '"');
				return false;
			}
		}

		nzTernary recognized = nzTernary_Unknown;
		if (fileLoader)
		{
			if (checkFunc)
			{
				file.SetCursorPos(0);

				recognized = checkFunc(file, parameters);
				if (recognized == nzTernary_False)
					continue;
				else
					found = true;
			}
			else
			{
				recognized = nzTernary_Unknown;
				found = true;
			}

			if (fileLoader(resource, filePath, parameters))
				return true;
		}
		else
		{
			file.SetCursorPos(0);

			recognized = checkFunc(file, parameters);
			if (recognized == nzTernary_False)
				continue;
			else if (recognized == nzTernary_True)
				found = true;

			file.SetCursorPos(0);

			if (streamLoader(resource, file, parameters))
				return true;
		}

		if (recognized == nzTernary_True)
			NazaraWarning("Loader failed");
	}

	if (found)
		NazaraError("Failed to load file: all loaders failed");
	else
		NazaraError("Failed to load file: no loader found");

	return false;
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters)
{
	NzMemoryStream stream(data, size);

	return LoadFromStream(resource, stream, parameters);
}

template<typename Type, typename Parameters>
bool NzResourceLoader<Type, Parameters>::LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters)
{
	#if NAZARA_CORE_SAFE
	if (!parameters.IsValid())
	{
		NazaraError("Invalid parameters");
		return false;
	}

	if (stream.GetSize() == 0 || stream.GetCursorPos() >= stream.GetSize())
	{
		NazaraError("No data to load");
		return false;
	}
	#endif

	nzUInt64 streamPos = stream.GetCursorPos();
	bool found = false;
	for (Loader& loader : Type::s_loaders)
	{
		StreamChecker checkFunc = std::get<1>(loader);
		StreamLoader streamLoader = std::get<2>(loader);

		stream.SetCursorPos(streamPos);

		// Le loader supporte-t-il les données ?
		nzTernary recognized = checkFunc(stream, parameters);
		if (recognized == nzTernary_False)
			continue;
		else if (recognized == nzTernary_True)
			found = true;

		// On repositionne le stream à son ancienne position
		stream.SetCursorPos(streamPos);

		// Chargement de la ressource
		if (streamLoader(resource, stream, parameters))
			return true;

		if (recognized == nzTernary_True)
			NazaraWarning("Loader failed");
	}

	if (found)
		NazaraError("Failed to load file: all loaders failed");
	else
		NazaraError("Failed to load file: no loader found");

	return false;
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::RegisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader)
{
	#if NAZARA_CORE_SAFE
	if (streamLoader)
	{
		if (!checkFunc)
		{
			NazaraError("StreamLoader present without StreamChecker");
			return;
		}
	}
	else if (!fileLoader)
	{
		NazaraError("Neither FileLoader nor StreamLoader were present");
		return;
	}
	#endif

	Type::s_loaders.push_front(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader));
}

template<typename Type, typename Parameters>
void NzResourceLoader<Type, Parameters>::UnregisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader)
{
	Type::s_loaders.remove(std::make_tuple(extensionGetter, checkFunc, streamLoader, fileLoader));
}

#include <Nazara/Core/DebugOff.hpp>
