#pragma once
#include <vector>
#include <atomic>
#include <memory>

#include "BlockingQueue.h"

namespace Samodiva
{
struct Task
{
public:
	const char* GetName() const
	{
		return m_Name;
	}

	virtual ~Task() {}
	virtual void Execute() = 0;

protected:
	Task(const char* name)
		: m_Name(name)
	{}

private:
	const char* m_Name;
};

template<typename Functor>
struct SimpleTask : public Task
{
public:
	SimpleTask(const char* name, Functor functor)
		: Task(name)
		, m_Functor(functor)
	{
	}

	virtual void Execute() override
	{
		m_Functor();
	}
private:
	Functor m_Functor;
};
using TaskPtr = StdUniquePtr<Task>;

class TaskChannel
{
public:
	inline void Enqueue(TaskPtr&& task)
	{
		m_Tasks.Enqueue(std::move(task));
	}

	inline void Burst()
	{
		TaskPtr task;
		do
		{
			task = std::move(m_Tasks.Dequeue());
			task->Execute();
		} while (m_KeepRunning);
	}
	inline void StopBursting()
	{
		m_KeepRunning = false;
	}
private:
	BlockingQueue<TaskPtr> m_Tasks;
	std::atomic_bool m_KeepRunning = true;
};


using ChannelId = unsigned char;
template<ChannelId ChannelCount>
class TaskSystem
{
public:
	TaskSystem::TaskSystem()
	{
		SpawnThreads();
	}

	TaskSystem::~TaskSystem()
	{
		for (auto channelId = ChannelId(0); channelId < ChannelCount; ++channelId)
		{
			SpawnTask("StopBurstingOnExit", channelId, [this, channelId]() { m_Channels[channelId].StopBursting(); });
		}
		for (auto& thread : m_ThreadPool)
		{
			thread.join();
		}
	}

	template<typename Functor>
	inline void SpawnTask(const char* name, ChannelId channel, Functor f)
	{
		m_Channels[channel].Enqueue(MakeUniqueStd<SimpleTask<Functor>>(name, f));
	}

private:
	StdVector<std::thread> m_ThreadPool;
	std::array<TaskChannel, ChannelCount> m_Channels;

	// Spawn a thread for each channel as of now
	void SpawnThreads()
	{
		m_ThreadPool.reserve(m_Channels.size());
		for (auto i = 0u; i < m_Channels.size(); ++i)
		{
			std::thread workerThread(&ThreadMain, this, i);
			m_ThreadPool.push_back(std::move(workerThread));
		}
	}

	static void ThreadMain(TaskSystem* self, ChannelId channel)
	{
		self->m_Channels[channel].Burst();
	}
};

}