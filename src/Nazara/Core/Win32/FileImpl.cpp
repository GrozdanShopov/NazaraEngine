// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/FileImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Win32/Time.hpp>
#include <Nazara/Core/Debug.hpp>

NzFileImpl::NzFileImpl(const NzFile* parent) :
m_endOfFile(false)
{
	NazaraUnused(parent);
}

NzFileImpl::~NzFileImpl()
{
}

void NzFileImpl::Close()
{
	CloseHandle(m_handle);
}

bool NzFileImpl::EndOfFile() const
{
	return m_endOfFile;
}

void NzFileImpl::Flush()
{
	if (!FlushFileBuffers(m_handle))
		NazaraError("Unable to flush file: " + NzGetLastSystemError());
}

nzUInt64 NzFileImpl::GetCursorPos() const
{
	LARGE_INTEGER zero;
	zero.QuadPart = 0;

	LARGE_INTEGER position;
	SetFilePointerEx(m_handle, zero, &position, FILE_CURRENT);

	return position.QuadPart;
}

bool NzFileImpl::Open(const NzString& filePath, unsigned int mode)
{
	DWORD access;
	DWORD shareMode = FILE_SHARE_READ;
	DWORD openMode;
	if (mode & NzFile::ReadOnly)
	{
		access = GENERIC_READ;
		openMode = OPEN_EXISTING;
	}
	else if (mode & NzFile::ReadWrite)
	{
		if (mode & NzFile::Append)
			access = FILE_APPEND_DATA;
		else
			access = GENERIC_READ | GENERIC_WRITE;

		if (mode & NzFile::Truncate)
			openMode = CREATE_ALWAYS;
		else
			openMode = OPEN_ALWAYS;
	}
	else if (mode & NzFile::WriteOnly)
	{
		if (mode & NzFile::Append)
			access = FILE_APPEND_DATA;
		else
			access = GENERIC_WRITE;

		if (mode & NzFile::Truncate)
			openMode = CREATE_ALWAYS;
		else
			openMode = OPEN_ALWAYS;
	}
	else
		return false;

	if ((mode & NzFile::Lock) == 0)
		shareMode |= FILE_SHARE_WRITE;

	wchar_t* path = filePath.GetWideBuffer();
	m_handle = CreateFileW(path, access, shareMode, nullptr, openMode, 0, nullptr);
	delete[] path;

	return m_handle != INVALID_HANDLE_VALUE;
}

std::size_t NzFileImpl::Read(void* buffer, std::size_t size)
{
	DWORD read = 0;
	if (ReadFile(m_handle, buffer, size, &read, nullptr))
	{
		m_endOfFile = (size != read);
		return read;
	}
	else
		return 0;
}

bool NzFileImpl::SetCursorPos(NzFile::CursorPosition pos, nzInt64 offset)
{
	DWORD moveMethod;
	switch (pos)
	{
		case NzFile::AtBegin:
			moveMethod = FILE_BEGIN;
			break;

		case NzFile::AtCurrent:
			moveMethod = FILE_CURRENT;
			break;

		case NzFile::AtEnd:
			moveMethod = FILE_END;
			break;

		default:
			NazaraInternalError("Cursor position not handled (0x" + NzString::Number(pos, 16) + ')');
			return false;
	}

	m_endOfFile = false;

	LARGE_INTEGER distance;
	distance.QuadPart = offset;

	return SetFilePointerEx(m_handle, distance, nullptr, moveMethod) != 0;
}

std::size_t NzFileImpl::Write(const void* buffer, std::size_t size)
{
	DWORD written = 0;

	LARGE_INTEGER cursorPos;
	cursorPos.QuadPart = GetCursorPos();

	LockFile(m_handle, cursorPos.LowPart, cursorPos.HighPart, size, 0);
	WriteFile(m_handle, buffer, size, &written, nullptr);
	UnlockFile(m_handle, cursorPos.LowPart, cursorPos.HighPart, size, 0);

	return written;
}

bool NzFileImpl::Copy(const NzString& sourcePath, const NzString& targetPath)
{
	wchar_t* path = sourcePath.GetWideBuffer();
	wchar_t* newPath = targetPath.GetWideBuffer();

	if (CopyFileW(path, newPath, false))
	{
		delete[] path;
		delete[] newPath;

		return true;
	}
	else
	{
		NazaraError("Unable to copy file: " + NzGetLastSystemError());
		delete[] path;
		delete[] newPath;

		return false;
	}
}

bool NzFileImpl::Delete(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();

	if (DeleteFileW(path))
	{
		delete[] path;

		return true;
	}
	else
	{
		NazaraError("Unable to delete file (" + filePath + "): " + NzGetLastSystemError());
		delete[] path;

		return false;
	}
}

bool NzFileImpl::Exists(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();
	HANDLE handle = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	delete[] path;

	if (handle == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle(handle);

	return true;
}

time_t NzFileImpl::GetCreationTime(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();
	HANDLE handle = CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	delete[] path;

	if (handle == INVALID_HANDLE_VALUE)
		return 0;

	FILETIME creationTime;
	if (!GetFileTime(handle, &creationTime, nullptr, nullptr))
	{
		NazaraError("Unable to get creation time: " + NzGetLastSystemError());

		CloseHandle(handle);
		return 0;
	}

	CloseHandle(handle);

	return FileTimeToTime(&creationTime);
}

time_t NzFileImpl::GetLastAccessTime(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();
	HANDLE handle = CreateFileW(path, 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	delete[] path;

	if (handle == INVALID_HANDLE_VALUE)
		return 0;

	FILETIME accessTime;
	if (!GetFileTime(handle, nullptr, &accessTime, nullptr))
	{
		NazaraError("Unable to get last access time: " + NzGetLastSystemError());

		CloseHandle(handle);
		return 0;
	}

	CloseHandle(handle);

	return FileTimeToTime(&accessTime);
}

time_t NzFileImpl::GetLastWriteTime(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();
	HANDLE handle = CreateFileW(path, 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	delete[] path;

	if (handle == INVALID_HANDLE_VALUE)
		return 0;

	FILETIME writeTime;
	if (!GetFileTime(handle, nullptr, nullptr, &writeTime))
	{
		NazaraError("Unable to get last write time: " + NzGetLastSystemError());
		CloseHandle(handle);

		return 0;
	}

	CloseHandle(handle);

	return FileTimeToTime(&writeTime);
}

nzUInt64 NzFileImpl::GetSize(const NzString& filePath)
{
	wchar_t* path = filePath.GetWideBuffer();
	HANDLE handle = CreateFileW(path, 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	delete[] path;

	if (handle == INVALID_HANDLE_VALUE)
		return 0;

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(handle, &fileSize))
		fileSize.QuadPart = 0;

	CloseHandle(handle);

	return fileSize.QuadPart;
}

bool NzFileImpl::Rename(const NzString& sourcePath, const NzString& targetPath)
{
	wchar_t* path = sourcePath.GetWideBuffer();
	wchar_t* newPath = targetPath.GetWideBuffer();

	bool success = MoveFileExW(path, newPath, MOVEFILE_COPY_ALLOWED) != 0;

	delete[] path;
	delete[] newPath;

	if (success)
		return true;
	else
	{
		NazaraError("Unable to rename file: " + NzGetLastSystemError());
		return false;
	}
}
