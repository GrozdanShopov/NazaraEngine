// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <pthread.h>
#endif

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

	bool initialized = false;
	const unsigned int magic = 0x51429EE;
	const char* MLTFileName = "NazaraLeaks.log";
	const char* nextFreeFile = "Internal error";
	unsigned int nextFreeLine = 0;

	Block ptrList =
	{
		0,
		nullptr,
		&ptrList,
		&ptrList,
		false,
		0,
		magic
	};

	#if defined(NAZARA_PLATFORM_WINDOWS)
	CRITICAL_SECTION mutex;
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
	if (!initialized)
		Initialize();

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&mutex);
	#endif

	Block* ptr = reinterpret_cast<Block*>(std::malloc(size+sizeof(Block)));
	if (!ptr)
		return nullptr; // Impossible d'envoyer une exception car cela allouerait de la m�moire avec new (boucle infinie)

	ptr->array = multi;
	ptr->file = file;
	ptr->line = line;
	ptr->size = size;
	ptr->magic = magic;

	ptr->prev = ptrList.prev;
	ptr->next = &ptrList;
	ptrList.prev->next = ptr;
	ptrList.prev = ptr;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&mutex);
	#endif

	return reinterpret_cast<char*>(ptr)+sizeof(Block);
}

void NzMemoryManager::Free(void* pointer, bool multi)
{
	if (!pointer)
		return;

	Block* ptr = reinterpret_cast<Block*>(reinterpret_cast<char*>(pointer)-sizeof(Block));
	if (ptr->magic != magic)
		return;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&mutex);
	#endif

	if (ptr->array != multi)
	{
		char* time = TimeInfo();

		FILE* log = std::fopen(MLTFileName, "a");

		if (nextFreeFile)
		{
			if (multi)
				std::fprintf(log, "%s Warning: delete[] after new at %s:%d\n", time, nextFreeFile, nextFreeLine);
			else
				std::fprintf(log, "%s Warning: delete after new[] at %s:%d\n", time, nextFreeFile, nextFreeLine);
		}
		else
		{
			if (multi)
				std::fprintf(log, "%s Warning: delete[] after new at unknown position\n", time);
			else
				std::fprintf(log, "%s Warning: delete after new[] at unknown position\n", time);
		}

		std::fclose(log);

		std::free(time);
	}

	ptr->magic = 0;
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;

	std::free(ptr);

	nextFreeFile = nullptr;
	nextFreeLine = 0;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&mutex);
	#endif
}

void NzMemoryManager::NextFree(const char* file, unsigned int line)
{
	nextFreeFile = file;
	nextFreeLine = line;
}

void NzMemoryManager::Initialize()
{
	char* time = TimeInfo();

	FILE* file = std::fopen(MLTFileName, "w");
	std::fprintf(file, "%s ==============================\n", time);
	std::fprintf(file, "%s   Nazara Memory Leak Tracker  \n", time);
	std::fprintf(file, "%s ==============================\n", time);
	std::fclose(file);

	std::free(time);

	if (std::atexit(Uninitialize) != 0)
	{
		static NzMemoryManager manager;
	}

	#ifdef NAZARA_PLATFORM_WINDOWS
	InitializeCriticalSection(&mutex);
	#endif

	initialized = true;
}

char* NzMemoryManager::TimeInfo()
{
	char* buffer = reinterpret_cast<char*>(std::malloc(23*sizeof(char)));

	time_t currentTime = std::time(nullptr);
	std::strftime(buffer, 23, "%d/%m/%Y - %H:%M:%S:", std::localtime(&currentTime));

	return buffer;
}

void NzMemoryManager::Uninitialize()
{
	#ifdef NAZARA_PLATFORM_WINDOWS
	DeleteCriticalSection(&mutex);
	#endif

	FILE* log = std::fopen(MLTFileName, "a");

	char* time = TimeInfo();

	std::fprintf(log, "%s Application finished, checking leaks...\n", time);

	if (ptrList.next == &ptrList)
	{
		std::fprintf(log, "%s ==============================\n", time);
		std::fprintf(log, "%s        No leak detected       \n", time);
		std::fprintf(log, "%s ==============================", time);
	}
	else
	{
		std::fprintf(log, "%s ==============================\n", time);
		std::fprintf(log, "%s    Leaks have been detected   \n", time);
		std::fprintf(log, "%s ==============================\n\n", time);
		std::fputs("Leak list:\n", log);

		Block* ptr = ptrList.next;
		unsigned int count = 0;
		unsigned int totalSize = 0;
		while (ptr != &ptrList)
		{
			count++;
			totalSize += ptr->size;
			if (ptr->file)
				std::fprintf(log, "-0x%p -> %d bytes allocated at %s:%d\n", reinterpret_cast<char*>(ptr)+sizeof(Block), ptr->size, ptr->file, ptr->line);
			else
				std::fprintf(log, "-0x%p -> %d bytes allocated at unknown position\n", reinterpret_cast<char*>(ptr)+sizeof(Block), ptr->size);

			void* pointer = ptr;
			ptr = ptr->next;
			std::free(pointer);
		}

		std::fprintf(log, "\n%d blocks leaked (%d bytes)", count, totalSize);
	}

	std::free(time);
	std::fclose(log);
}

void* operator new(std::size_t size, const char* file, unsigned int line)
{
	return NzMemoryManager::Allocate(size, false, file, line);
}

void* operator new[](std::size_t size, const char* file, unsigned int line)
{
	return NzMemoryManager::Allocate(size, true, file, line);
}

void operator delete(void* ptr, const char* file, unsigned int line) throw()
{
	NzMemoryManager::NextFree(file, line);
	NzMemoryManager::Free(ptr, false);
}

void operator delete[](void* ptr, const char* file, unsigned int line) throw()
{
	NzMemoryManager::NextFree(file, line);
	NzMemoryManager::Free(ptr, true);
}
