// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <atomic>
#include <queue>
#include <vector>
#include <Nazara/Core/Debug.hpp>

///FIXME: Revoir tout ça

namespace
{
	struct TaskSchedulerImpl
	{
		std::queue<NzFunctor*> tasks;
		std::vector<NzThread> workers;
		NzConditionVariable waiterConditionVariable;
		NzConditionVariable workerConditionVariable;
		NzMutex taskMutex;
		NzMutex taskCountMutex;
		NzMutex waiterConditionVariableMutex;
		NzMutex workerConditionVariableMutex;
		volatile bool running = true;
		std::atomic<unsigned int> taskCount;
	};

	TaskSchedulerImpl* s_impl = nullptr;
	unsigned int s_workerCount = 0;

	void WorkerFunc()
	{
		do
		{
			NzFunctor* task;
			{
				NzLockGuard lock(s_impl->taskMutex);
				if (!s_impl->tasks.empty())
				{
					task = s_impl->tasks.front();
					s_impl->tasks.pop();
				}
				else
					task = nullptr;
			}

			// Avons-nous une tâche ?
			if (task)
			{
				task->Run(); // Chouette ! Allons travailler gaiement

				delete task; // Sans oublier de supprimer la tâche

				s_impl->taskCountMutex.Lock();
				#ifdef NAZARA_DEBUG
				if (s_impl->taskCount == 0)
					NazaraInternalError("Task count is already 0");
				#endif

				if (--s_impl->taskCount == 0)
				{
					// On peut signaler la fin du travail
					s_impl->waiterConditionVariableMutex.Lock();
					s_impl->waiterConditionVariable.Signal();
					s_impl->waiterConditionVariableMutex.Unlock();
				}
				s_impl->taskCountMutex.Unlock();
			}
			else
			{
				// Nous attendons qu'une nouvelle tâche arrive
				s_impl->workerConditionVariableMutex.Lock();
				s_impl->workerConditionVariable.Wait(&s_impl->workerConditionVariableMutex);
				s_impl->workerConditionVariableMutex.Unlock();
			}
		}
		while (s_impl->running);
	}
}

unsigned int NzTaskScheduler::GetWorkerCount()
{
	return (s_workerCount > 0) ? s_workerCount : NzHardwareInfo::GetProcessorCount();
}

bool NzTaskScheduler::Initialize()
{
	if (s_impl)
		return true; // Déjà initialisé

	s_impl = new TaskSchedulerImpl;

	unsigned int workerCount = GetWorkerCount();

	s_impl->workers.resize(workerCount);
	for (unsigned int i = 0; i < workerCount; ++i)
		s_impl->workers[i] = NzThread(WorkerFunc);

	return true;
}

void NzTaskScheduler::SetWorkerCount(unsigned int workerCount)
{
	s_workerCount = workerCount;
	if (s_impl)
	{
		unsigned int newWorkerCount = GetWorkerCount();
		unsigned int oldWorkerCount = s_impl->workers.size();
		s_impl->workers.resize(newWorkerCount);
		if (newWorkerCount > oldWorkerCount)
		{
			for (unsigned int i = oldWorkerCount-1; i < newWorkerCount; ++i)
				s_impl->workers[i] = NzThread(WorkerFunc);
		}
	}
}

void NzTaskScheduler::Uninitialize()
{
	if (s_impl)
	{
		s_impl->running = false;

		// S'il reste des tâches en cours, on les libère
		{
			NzLockGuard lock(s_impl->taskMutex);
			while (!s_impl->tasks.empty())
			{
				delete s_impl->tasks.front();
				s_impl->tasks.pop();
			}
		}

		// Ensuite on réveille les threads pour qu'ils s'arrêtent d'eux-même
		s_impl->workerConditionVariableMutex.Lock();
		s_impl->workerConditionVariable.SignalAll();
		s_impl->workerConditionVariableMutex.Unlock();

		for (NzThread& thread : s_impl->workers)
			thread.Join();

		delete s_impl;
		s_impl = nullptr;
	}
}

void NzTaskScheduler::WaitForTasks()
{
	#ifdef NAZARA_CORE_SAFE
	if (!s_impl)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	// Tout d'abord, il y a-t-il des tâches en attente ?
	if (s_impl->tasks.empty())
		return;

	s_impl->taskCount = s_impl->tasks.size();

	// On verrouille d'abord la mutex entourant le signal (Pour ne pas perdre le signal en chemin)
	s_impl->waiterConditionVariableMutex.Lock();

	// Et ensuite seulement on réveille les worker
	s_impl->workerConditionVariableMutex.Lock();
	s_impl->workerConditionVariable.SignalAll();
	s_impl->workerConditionVariableMutex.Unlock();

	s_impl->waiterConditionVariable.Wait(&s_impl->waiterConditionVariableMutex);
	s_impl->waiterConditionVariableMutex.Unlock();
}

void NzTaskScheduler::AddTaskFunctor(NzFunctor* taskFunctor)
{
	#ifdef NAZARA_CORE_SAFE
	if (!s_impl)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	s_impl->tasks.push(taskFunctor);
}
