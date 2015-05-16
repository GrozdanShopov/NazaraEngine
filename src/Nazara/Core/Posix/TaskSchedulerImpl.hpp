// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TASKSCHEDULERIMPL_HPP
#define NAZARA_TASKSCHEDULERIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <atomic>
#include <memory>
#include <pthread.h>
#include <queue>

class NzTaskSchedulerImpl
{
	public:
		NzTaskSchedulerImpl() = delete;
		~NzTaskSchedulerImpl() = delete;

		static bool Initialize(unsigned int workerCount);
		static bool IsInitialized();
		static void Run(NzFunctor** tasks, unsigned int count);
		static void Uninitialize();
		static void WaitForTasks();

	private:
		static NzFunctor* PopQueue();
		static void Wait();
		static void* WorkerProc(void* userdata);

		static std::queue<NzFunctor*> s_tasks;
		static std::unique_ptr<pthread_t[]> s_threads;
		static std::atomic<bool> s_isDone;
		static std::atomic<bool> s_isWaiting;
		static std::atomic<bool> s_shouldFinish;
		static unsigned int s_workerCount;

		static pthread_mutex_t s_mutexQueue;
		static pthread_cond_t s_cvEmpty;
		static pthread_cond_t s_cvNotEmpty;
		static pthread_barrier_t s_barrier;
};

#endif // NAZARA_TASKSCHEDULERIMPL_HPP
