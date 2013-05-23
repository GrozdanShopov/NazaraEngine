// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DirectoryImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

NzDirectoryImpl::NzDirectoryImpl(const NzDirectory* parent)
{
	NazaraUnused(parent);
}

void NzDirectoryImpl::Close()
{
	FindClose(m_handle);
}

NzString NzDirectoryImpl::GetResultName() const
{
	return NzString::Unicode(m_result.cFileName);
}

nzUInt64 NzDirectoryImpl::GetResultSize() const
{
	LARGE_INTEGER size;
	size.HighPart = m_result.nFileSizeHigh;
	size.LowPart = m_result.nFileSizeLow;

	return size.QuadPart;
}

bool NzDirectoryImpl::IsResultDirectory() const
{
	if (m_result.dwFileAttributes != INVALID_FILE_ATTRIBUTES)
		return (m_result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	else
		return false;
}

bool NzDirectoryImpl::NextResult()
{
	if (m_firstCall) // Nous devons ignorer le premier appel car FindFirstFile nous a déjà renvoyé des résultats
	{
		m_firstCall = false;
		return true;
	}

	if (FindNextFileW(m_handle, &m_result))
		return true;
	else
	{
		if (GetLastError() != ERROR_NO_MORE_FILES)
			NazaraError("Unable to get next result: " + NzGetLastSystemError());

		return false;
	}
}

bool NzDirectoryImpl::Open(const NzString& dirPath)
{
	NzString searchPath = dirPath + "\\*";

	std::unique_ptr<wchar_t[]> wPath(searchPath.GetWideBuffer());
	m_handle = FindFirstFileW(wPath.get(), &m_result);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		NazaraError("Unable to open directory: " + NzGetLastSystemError());
		return false;
	}

	m_firstCall = true;

	return true;
}

bool NzDirectoryImpl::Create(const NzString& dirPath)
{
	std::unique_ptr<wchar_t[]> wPath(dirPath.GetWideBuffer());

	return (CreateDirectoryW(wPath.get(), nullptr) != 0) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool NzDirectoryImpl::Exists(const NzString& dirPath)
{
	std::unique_ptr<wchar_t[]> wPath(dirPath.GetWideBuffer());
	DWORD attributes = GetFileAttributesW(wPath.get());

	if (attributes != INVALID_FILE_ATTRIBUTES)
		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	else
		return false;
}

NzString NzDirectoryImpl::GetCurrent()
{
	NzString currentPath;
	std::unique_ptr<wchar_t[]> path(new wchar_t[MAX_PATH]);

	unsigned int size = GetCurrentDirectoryW(MAX_PATH, path.get());
	if (size > MAX_PATH) // La taille prends en compte le caractère nul
	{
		path.reset(new wchar_t[size]);
		if (GetCurrentDirectoryW(size, path.get()) != 0)
			currentPath = NzString::Unicode(path.get());
		else
			NazaraError("Unable to get current directory: " + NzGetLastSystemError());
	}
	else if (size == 0)
		NazaraError("Unable to get current directory: " + NzGetLastSystemError());
	else
		currentPath = NzString::Unicode(path.get());

	return currentPath;
}

bool NzDirectoryImpl::Remove(const NzString& dirPath)
{
	std::unique_ptr<wchar_t[]> path(dirPath.GetWideBuffer());
	bool success = RemoveDirectoryW(path.get()) != 0;

	DWORD error = GetLastError();
	return success || error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
}
