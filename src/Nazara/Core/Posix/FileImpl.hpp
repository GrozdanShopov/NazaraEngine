// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILEIMPL_HPP
#define NAZARA_FILEIMPL_HPP

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/File.hpp>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace Nz
{
	class File;
	class String;

	class FileImpl
	{
		public:
			FileImpl(const File* parent);
			FileImpl(const FileImpl&) = delete;
			FileImpl(FileImpl&&) = delete; ///TODO
			~FileImpl() = default;

			void Close();
			bool EndOfFile() const;
			void Flush();
			UInt64 GetCursorPos() const;
			bool Open(const String& filePath, UInt32 mode);
			std::size_t Read(void* buffer, std::size_t size);
			bool SetCursorPos(CursorPosition pos, Int64 offset);
			bool SetSize(UInt64 size);
			std::size_t Write(const void* buffer, std::size_t size);

			FileImpl& operator=(const FileImpl&) = delete;
			FileImpl& operator=(FileImpl&&) = delete; ///TODO

			static bool Copy(const String& sourcePath, const String& targetPath);
			static bool Delete(const String& filePath);
			static bool Exists(const String& filePath);
			static time_t GetCreationTime(const String& filePath);
			static time_t GetLastAccessTime(const String& filePath);
			static time_t GetLastWriteTime(const String& filePath);
			static UInt64 GetSize(const String& filePath);
			static bool Rename(const String& sourcePath, const String& targetPath);

		private:
			int m_fileDescriptor;
			mutable bool m_endOfFile;
			mutable bool m_endOfFileUpdated;
	};
}

#endif // NAZARA_FILEIMPL_HPP
