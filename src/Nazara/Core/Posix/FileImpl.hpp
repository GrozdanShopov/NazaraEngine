// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILEIMPL_HPP
#define NAZARA_FILEIMPL_HPP

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <ctime>
#include <filesystem>

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

			bool EndOfFile() const;
			void Flush();
			UInt64 GetCursorPos() const;
			bool Open(const std::filesystem::path& filePath, OpenModeFlags mode);
			std::size_t Read(void* buffer, std::size_t size);
			bool SetCursorPos(CursorPosition pos, Int64 offset);
			bool SetSize(UInt64 size);
			std::size_t Write(const void* buffer, std::size_t size);

			FileImpl& operator=(const FileImpl&) = delete;
			FileImpl& operator=(FileImpl&&) = delete; ///TODO

		private:
			int m_fileDescriptor;
			mutable bool m_endOfFile;
			mutable bool m_endOfFileUpdated;
	};
}

#endif // NAZARA_FILEIMPL_HPP
