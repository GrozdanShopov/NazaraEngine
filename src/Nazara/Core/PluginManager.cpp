// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PluginManager.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <list>
#include <memory>
#include <unordered_map>
#include <Nazara/Core/Debug.hpp>

namespace
{
	using PluginLoad = bool (*)();
	using PluginUnload = void (*)();

	std::list<NzString> s_directories;
	std::unordered_map<NzString, NzDynLib*> s_plugins;

	NzString s_pluginFiles[] =
	{
		"NazaraAssimp",  // nzPlugin_Assimp
		"NazaraFreetype" // nzPlugin_FreeType
	};
}

bool NzPluginManager::AddDirectory(const NzString& directoryPath)
{
	NzString path = NzFile::AbsolutePath(directoryPath);

	if (!NzDirectory::Exists(path))
	{
		NazaraWarning("Directory not existing: " + directoryPath);
		return false;
	}

	s_directories.push_back(path);

	return true;
}

bool NzPluginManager::Initialize()
{
	s_directories.push_back(NzFile::AbsolutePath("."));

	NzString libDir = NzFile::AbsolutePath("lib");
	if (NzDirectory::Exists(libDir))
		s_directories.push_back(libDir);

	return true;
}

bool NzPluginManager::Mount(nzPlugin plugin)
{
	return Mount(s_pluginFiles[plugin]);
}

bool NzPluginManager::Mount(const NzString& pluginPath)
{
	NzString path = pluginPath;
	if (!path.EndsWith(NAZARA_DYNLIB_EXTENSION))
		path += NAZARA_DYNLIB_EXTENSION;

	bool exists = false;
	if (!NzFile::IsAbsolute(path))
	{
		for (const NzString& dir : s_directories)
		{
			NzString testPath;
			testPath.Reserve(dir.GetSize() + path.GetSize() + 10);

			testPath = dir;
			testPath += NAZARA_DIRECTORY_SEPARATOR;
			testPath += path;

			if (NzFile::Exists(testPath))
			{
				path = testPath;
				exists = true;
				break;
			}
		}
	}
	else if (NzFile::Exists(path))
		exists = true;

	if (!exists)
	{
		NazaraError("Failed to find plugin file");
		return false;
	}

	std::unique_ptr<NzDynLib> library(new NzDynLib(path));
	if (!library->Load())
	{
		NazaraError("Failed to load plugin");
		return false;
	}

	PluginLoad func = reinterpret_cast<PluginLoad>(library->GetSymbol("NzPluginLoad"));
	if (!func)
	{
		NazaraError("Failed to get symbol NzPluginLoad");
		return false;
	}

	if (!func())
	{
		NazaraError("Plugin failed to load");
		return false;
	}

	s_plugins[pluginPath] = library.release();

	return true;
}

void NzPluginManager::RemoveDirectory(const NzString& directoryPath)
{
	s_directories.remove(directoryPath);
}

void NzPluginManager::Unmount(nzPlugin plugin)
{
	Unmount(s_pluginFiles[plugin]);
}

void NzPluginManager::Unmount(const NzString& pluginPath)
{
	auto it = s_plugins.find(pluginPath);
	if (it == s_plugins.end())
	{
		NazaraError("Plugin not loaded");
		return;
	}

	PluginUnload func = reinterpret_cast<PluginUnload>(it->second->GetSymbol("NzPluginUnload"));
	if (func)
		func();

	it->second->Unload();

	s_plugins.erase(it);
}

void NzPluginManager::Uninitialize()
{
	s_directories.clear();

	for (auto& pair : s_plugins)
	{
		PluginUnload func = reinterpret_cast<PluginUnload>(pair.second->GetSymbol("NzPluginUnload"));
		if (func)
			func();

		pair.second->Unload();
	}

	s_plugins.clear();
}
