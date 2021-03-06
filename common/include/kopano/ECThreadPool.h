/*
 * SPDX-License-Identifier: AGPL-3.0-only
 * Copyright 2005 - 2016 Zarafa and its licensors
 */
#pragma once
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <pthread.h>
#include <list>
#include <kopano/zcdefs.h>
#include <kopano/timeutil.hpp>

namespace KC {

class Config;
class ECTask;
class ECThreadPool;
class ECWatchdog;

class KC_EXPORT ECThreadWorker {
	public:
	ECThreadWorker(ECThreadPool *);
	virtual ~ECThreadWorker() = default;
	virtual bool init() { return true; }
	virtual void exit() {}

	ECThreadPool *m_pool = nullptr;
};

/**
 * This class represents a thread pool with a fixed amount of worker threads.
 * The amount of workers can be modified at run time, but is not automatically
 * adjusted based on the task queue length or age.
 */
class KC_EXPORT ECThreadPool {
	protected:
	struct STaskInfo {
		ECTask			*lpTask;
		KC::time_point enq_stamp;
		bool			bDelete;
	};

	typedef std::map<pthread_t, std::shared_ptr<ECThreadWorker>> ThreadSet;
	typedef std::list<STaskInfo> TaskList;

public:
	ECThreadPool(const std::string &name, unsigned int spares);
	virtual ~ECThreadPool();
	void enable_watchdog(bool, std::shared_ptr<Config> = {});
	bool enqueue(ECTask *lpTask, bool bTakeOwnership = false, time_point *enq_time = nullptr);
	void set_thread_count(unsigned int spares, unsigned int tmax = 0, bool wait = false);
	void add_extra_thread();
	time_duration front_item_age() const;
	size_t queue_length() const;
	void thread_counts(size_t *active, size_t *idle) const;

	std::string m_poolname = "noname";

	protected:
	virtual std::unique_ptr<ECThreadWorker> make_worker();
	KC_HIDDEN size_t threadCount() const; /* unlocked variant */
	KC_HIDDEN bool getNextTask(STaskInfo *, std::unique_lock<std::mutex> &);
	KC_HIDDEN void joinTerminated(std::unique_lock<std::mutex> &);
	KC_HIDDEN HRESULT create_thread_unlocked();
	KC_HIDDEN static void *threadFunc(void *);

	ThreadSet m_setThreads, m_setTerminated;
	TaskList	m_listTasks;

	mutable std::mutex m_hMutex;
	std::condition_variable m_hCondition, m_hCondTerminated;
	mutable std::condition_variable m_hCondTaskDone;
	std::atomic<size_t> m_active{0}, m_ulTermReq{0};
	std::atomic<size_t> m_threads_spares{0}, m_threads_max{0};
	std::unique_ptr<ECWatchdog> m_watchdog;

	ECThreadPool(const ECThreadPool &) = delete;
	ECThreadPool &operator=(const ECThreadPool &) = delete;
};

/**
 * This class represents a task that can be queued on an ECThreadPool or
 * derived object.
 * Once the threadpool has a free worker and all previously queued tasks have
 * been processed, the task will be dispatched and its "run" method
 * executed.
 * There is no way of knowing when the task is done.
 */
class KC_EXPORT ECTask {
public:
	virtual ~ECTask() = default;
	virtual void execute();
	ECThreadWorker *m_worker = nullptr;

protected:
	virtual void run() = 0;
	ECTask() = default;

private:
	// Make the object non-copyable
	ECTask(const ECTask &) = delete;
	ECTask &operator=(const ECTask &) = delete;
};

/**
 * This class represents a task that can be executed on an ECThreadPool or
 * derived object. It's similar to an ECTask, but one can wait for the task
 * to be finished.
 */
class KC_EXPORT ECWaitableTask : public ECTask {
public:
	static const unsigned WAIT_INFINITE = (unsigned)-1;

	enum State {
		Idle = 1,
		Running = 2,
		Done = 4
	};

	virtual ~ECWaitableTask();
	virtual void execute() override;
	bool done() const { return m_state == Done; }
	bool wait(unsigned timeout = WAIT_INFINITE, unsigned waitMask = Done) const;

protected:
	ECWaitableTask();

private:
	mutable std::mutex m_hMutex;
	mutable std::condition_variable m_hCondition;
	State					m_state;
};

/**
 * This class can be used to run a function with one argument asynchronously on
 * an ECThreadPool or derived class.
 * To call a function with more than one argument boost::bind can be used.
 */
template<typename Rt, typename Fn, typename At>
class ECDeferredFunc KC_FINAL : public ECWaitableTask {
public:
	/**
	 * Construct an ECDeferredFunc instance.
	 * @param[in]	fn		The function to execute
	 * @param[in]	arg		The argument to pass to fn.
	 */
	ECDeferredFunc(Fn fn, const At &arg) : m_fn(fn), m_arg(arg)
	{ }

	virtual void run() override { m_result = m_fn(m_arg); }

	/**
	 * Get the result of the asynchronous function. This method will
	 * block until the method has been executed.
	 */
	Rt result() const
	{
		wait();
		return m_result;
	}

private:
	Rt m_result = 0;
	Fn m_fn;
	At m_arg;
};

} /* namespace */
