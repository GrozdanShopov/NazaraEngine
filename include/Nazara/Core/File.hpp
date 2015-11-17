// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILE_HPP
#define NAZARA_FILE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/OutputStream.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_FILE
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <ctime>

namespace Nz
{
	class FileImpl;

	class NAZARA_CORE_API File : public InputStream, public OutputStream
	{
		public:
			File();
			File(const String& filePath);
			File(const String& filePath, UInt32 openMode);
			File(const File&) = delete;
			File(File&& file) noexcept;
			~File();

			bool Copy(const String& newFilePath);
			void Close();

			bool Delete();

			bool EndOfFile() const;
			bool EndOfStream() const override;

			bool Exists() const;

			void Flush() override;

			time_t GetCreationTime() const;
			UInt64 GetCursorPos() const override;
			String GetDirectory() const override;
			String GetFileName() const;
			time_t GetLastAccessTime() const;
			time_t GetLastWriteTime() const;
			String GetPath() const override;
			UInt64 GetSize() const override;

			bool IsOpen() const;

			bool Open(unsigned int openMode = OpenMode_Current);
			bool Open(const String& filePath, unsigned int openMode = OpenMode_Current);

			std::size_t Read(void* buffer, std::size_t size) override;
			std::size_t Read(void* buffer, std::size_t typeSize, unsigned int count);
			bool Rename(const String& newFilePath);

			bool SetCursorPos(CursorPosition pos, Int64 offset = 0);
			bool SetCursorPos(UInt64 offset) override;
			void SetEndianness(Endianness endianness);
			bool SetFile(const String& filePath);
			bool SetOpenMode(UInt32 openMode);

			using OutputStream::Write;
			std::size_t Write(const void* buffer, std::size_t size) override;
			std::size_t Write(const void* buffer, std::size_t typeSize, unsigned int count);

			File& operator=(const String& filePath);
			File& operator=(const File&) = delete;
			File& operator=(File&& file) noexcept;

			static String AbsolutePath(const String& filePath);
			static inline ByteArray ComputeHash(HashType hash, const String& filePath);
			static inline ByteArray ComputeHash(AbstractHash* hash, const String& filePath);
			static bool Copy(const String& sourcePath, const String& targetPath);
			static bool Delete(const String& filePath);
			static bool Exists(const String& filePath);
			static time_t GetCreationTime(const String& filePath);
			static String GetDirectory(const String& filePath);
			static time_t GetLastAccessTime(const String& filePath);
			static time_t GetLastWriteTime(const String& filePath);
			static UInt64 GetSize(const String& filePath);
			static bool IsAbsolute(const String& filePath);
			static String NormalizePath(const String& filePath);
			static String NormalizeSeparators(const String& filePath);
			static bool Rename(const String& sourcePath, const String& targetPath);

		private:
			NazaraMutexAttrib(m_mutex, mutable)

			Endianness m_endianness;
			String m_filePath;
			FileImpl* m_impl;
	};

	template<>
	struct Hashable<File>;
}

#include <Nazara/Core/File.inl>

#endif // NAZARA_FILE_HPP
