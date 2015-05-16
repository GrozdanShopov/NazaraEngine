// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryManager.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <new>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <pthread.h>
#endif

// Le seul fichier n'ayant pas à inclure Debug.hpp

namespace
{
	struct Block
	{
		std::size_t size;
		const char* file;
		Block* prev;
		Block* next;
		bool array;
		unsigned int line;
		unsigned int magic;
	};

	bool s_allocationLogging = false;
	bool s_initialized = false;
	const unsigned int s_magic = 0xDEADB33FUL;
	const char* s_logFileName = "NazaraMemory.log";
	thread_local const char* s_nextFreeFile = "(Internal error)";
	thread_local unsigned int s_nextFreeLine = 0;

	Block s_list =
	{
		0,
		nullptr,
		&s_list,
		&s_list,
		false,
		0,
		s_magic
	};

	unsigned int s_allocationCount = 0;
	unsigned int s_allocatedBlock = 0;
	std::size_t s_allocatedSize = 0;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	CRITICAL_SECTION s_mutex;
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
	#endif
}

NzMemoryManager::NzMemoryManager()
{
}

NzMemoryManager::~NzMemoryManager()
{
	Uninitialize();
}

void* NzMemoryManager::Allocate(std::size_t size, bool multi, const char* file, unsigned int line)
{
	if (!s_initialized)
		Initialize();

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&s_mutex);
	#endif

	Block* ptr = reinterpret_cast<Block*>(std::malloc(size+sizeof(Block)));
	if (!ptr)
	{
		char timeStr[23];
		TimeInfo(timeStr);

		FILE* log = std::fopen(s_logFileName, "a");

		if (file)
			std::fprintf(log, "%s Failed to allocate %zu bytes at %s:%u\n", timeStr, size, file, line);
		else
			std::fprintf(log, "%s Failed to allocate %zu bytes at unknown position\n", timeStr, size);

		std::fclose(log);

		throw std::bad_alloc();
		return nullptr; // Ça me rassure d'avoir un return, aussi inutile soit-il
	}

	ptr->array = multi;
	ptr->file = file;
	ptr->line = line;
	ptr->size = size;
	ptr->magic = s_magic;

	ptr->prev = s_list.prev;
	ptr->next = &s_list;
	s_list.prev->next = ptr;
	s_list.prev = ptr;

	s_allocatedBlock++;
	s_allocatedSize += size;
	s_allocationCount++;

	if (s_allocationLogging)
	{
		char timeStr[23];
 		TimeInfo(timeStr);

		FILE* log = std::fopen(s_logFileName, "a");

		if (file)
			std::fprintf(log, "%s Allocated %zu bytes at %s:%u\n", timeStr, size, file, line);
		else
			std::fprintf(log, "%s Allocated %zu bytes at unknown position\n", timeStr, size);

		std::fclose(log);
	}

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&s_mutex);
	#endif

	return reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block);
}

void NzMemoryManager::EnableAllocationLogging(bool logAllocations)
{
	s_allocationLogging = logAllocations;
}

void NzMemoryManager::Free(void* pointer, bool multi)
{
	if (!pointer)
		return;

	Block* ptr = reinterpret_cast<Block*>(reinterpret_cast<nzUInt8*>(pointer) - sizeof(Block));
	if (ptr->magic != s_magic)
		return;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&s_mutex);
	#endif

	if (ptr->array != multi)
	{
		char timeStr[23];
		TimeInfo(timeStr);

		FILE* log = std::fopen(s_logFileName, "a");

		const char* error = (multi) ? "delete[] after new" : "delete after new[]";
		if (s_nextFreeFile)
			std::fprintf(log, "%s Warning: %s at %s:%u\n", timeStr, error, s_nextFreeFile, s_nextFreeLine);
		else
			std::fprintf(log, "%s Warning: %s at unknown position\n", error, timeStr);

		std::fclose(log);
	}

	ptr->magic = 0; // Évitons des problèmes
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;

	s_allocatedBlock--;
	s_allocatedSize -= ptr->size;

	std::free(ptr);

	s_nextFreeFile = nullptr;
	s_nextFreeLine = 0;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&s_mutex);
	#endif
}

unsigned int NzMemoryManager::GetAllocatedBlockCount()
{
	return s_allocatedBlock;
}

std::size_t NzMemoryManager::GetAllocatedSize()
{
	return s_allocatedSize;
}

unsigned int NzMemoryManager::GetAllocationCount()
{
	return s_allocationCount;
}

bool NzMemoryManager::IsAllocationLoggingEnabled()
{
	return s_allocationLogging;
}

void NzMemoryManager::NextFree(const char* file, unsigned int line)
{
	s_nextFreeFile = file;
	s_nextFreeLine = line;
}

void NzMemoryManager::Initialize()
{
	char timeStr[23];
	TimeInfo(timeStr);

	FILE* file = std::fopen(s_logFileName, "w");
	std::fprintf(file, "%s ==============================\n", timeStr);
	std::fprintf(file, "%s   Nazara Memory Leak Tracker  \n", timeStr);
	std::fprintf(file, "%s ==============================\n", timeStr);
	std::fclose(file);

	if (std::atexit(Uninitialize) != 0)
	{
		static NzMemoryManager manager;
	}

	#ifdef NAZARA_PLATFORM_WINDOWS
	InitializeCriticalSection(&s_mutex);
	//#elif defined(NAZARA_PLATFORM_POSIX) is already done in the namespace
	#endif

	s_initialized = true;
}

void NzMemoryManager::TimeInfo(char buffer[23])
{
	time_t currentTime = std::time(nullptr);
	std::strftime(buffer, 23, "%d/%m/%Y - %H:%M:%S:", std::localtime(&currentTime));
}

void NzMemoryManager::Uninitialize()
{
	#ifdef NAZARA_PLATFORM_WINDOWS
	DeleteCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_destroy(&s_mutex);
	#endif

	FILE* log = std::fopen(s_logFileName, "a");

	char timeStr[23];
	TimeInfo(timeStr);

	std::fprintf(log, "%s Application finished, checking leaks...\n", timeStr);

	if (s_allocatedBlock == 0)
	{
		std::fprintf(log, "%s ==============================\n", timeStr);
		std::fprintf(log, "%s        No leak detected       \n", timeStr);
		std::fprintf(log, "%s ==============================", timeStr);
	}
	else
	{
		std::fprintf(log, "%s ==============================\n", timeStr);
		std::fprintf(log, "%s    Leaks have been detected   \n", timeStr);
		std::fprintf(log, "%s ==============================\n\n", timeStr);
		std::fputs("Leak list:\n", log);

		Block* ptr = s_list.next;
		while (ptr != &s_list)
		{
			if (ptr->file)
				std::fprintf(log, "-0x%p -> %zu bytes allocated at %s:%u\n", reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block), ptr->size, ptr->file, ptr->line);
			else
				std::fprintf(log, "-0x%p -> %zu bytes allocated at unknown position\n", reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block), ptr->size);

			void* pointer = ptr;
			ptr = ptr->next;

			std::free(pointer);
		}

		std::fprintf(log, "\n%u blocks leaked (%zu bytes)", s_allocatedBlock, s_allocatedSize);
	}

	std::fclose(log);
}
