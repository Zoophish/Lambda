#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <vector>
#include <queue>
#include "Delegate.h"

template<class T>
class ThreadsafeQueue {
	private:
		mutable std::mutex mutex;
		std::condition_variable condition;
		std::queue<T> queue;

	public:
		ThreadsafeQueue() {}

		void Push(const T &_val) {
			std::lock_guard<std::mutex> lock(mutex);
			queue.push(_val);
			condition.notify_one();
		}

		bool TryPop(T &_val) {
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty()) return false;
			_val = std::move(queue.front());
			queue.pop();
			return true;
		}

		void WaitToPop(T &_val) {
			std::unique_lock<std::mutex> lock(mutex);
			condition.wait(lock, [this]() {
				return !queue.empty();
			});
			_val = std::move(queue.front());
			queue.pop();
		}

		bool Empty() const {
			std::lock_guard<std::mutex> lock(mutex);
			return queue.empty();
		}
};

class Task {
	private:
		struct ImplBase {
			virtual void Call() = 0;
			virtual ~ImplBase() {}
		};

		template<class Ret, typename... params>
		struct ImplType : ImplBase {
			typedef std::function<Ret(params...)> F;
			F func;

			ImplType(F &_func) {
				func = std::move(_func);
			}

			void Call() override {
				func();
			}
		};

		std::unique_ptr<ImplBase> impl;
		std::vector<const Task *> wait_for;
		mutable std::mutex mutex;
		mutable std::condition_variable wait_condition;
		std::atomic<bool> is_done;

	public:
		Task() {
			impl = nullptr;
			is_done = true;
		}

		template<class Ret, typename... params>
		static Task *MakeTask(std::function<Ret(params...)> &_func) {
			Task *task = new Task;
			ImplBase *newBase = new ImplType<Ret, params...>(_func);
			task->impl.reset(newBase);
			task->is_done = false;
			return task;
		}

		void Wait() const {
			std::unique_lock<std::mutex> lock(mutex);
			wait_condition.wait(lock, [this]() {
				return static_cast<bool>(is_done);
			});
		}

		void WaitFor(const Task &_task) {
			std::lock_guard<std::mutex> lock(mutex);
			wait_for.push_back(&_task);
		}

		void operator()() {
			{
				std::lock_guard<std::mutex> lock(mutex);
				const unsigned s = wait_for.size();
				for (unsigned i = 0; i < s; ++i) {
					wait_for[i]->Wait();
				}
				impl->Call();
				is_done = true;
			}
			wait_condition.notify_all();
		}

		bool IsDone() const {
			return static_cast<bool>(is_done);
		}
};

class ThreadPool {
	private:
		std::vector<std::thread> threads;
		ThreadsafeQueue<std::shared_ptr<Task>> work_queue;
		std::atomic<bool> done;
		unsigned num_threads;

		void RunWorker();

	public:
		/* 0 = hardware threads */
		ThreadPool(unsigned _num_threads = 0);

		void Enqueue(std::shared_ptr<Task> &_task);

		void Abort();

		void Start();

		~ThreadPool();
};