#pragma once
#include "Concurrency.h"

void ThreadPool::RunWorker() {
	while (!done) {
		std::shared_ptr<Task> task;
		work_queue.WaitToPop(task);
		if (!done) {
			task->operator()();
		}
		else {
			Enqueue(task);
		}
	}
}

ThreadPool::ThreadPool(unsigned _num_threads) {
	done = false;
	if (_num_threads == 0) {
		_num_threads = std::thread::hardware_concurrency();
	}
	try {
		threads.reserve(_num_threads);
		for (unsigned i = 0; i < _num_threads; ++i) {
			threads.push_back(std::thread(&ThreadPool::RunWorker, this));
		}
	}
	catch (...) {
		done = true;
		throw;
	}
}

void ThreadPool::Enqueue(std::shared_ptr<Task> &_task) {
	work_queue.Push(_task);
}

void ThreadPool::Abort() {
	done = true;
}

void ThreadPool::Start() {
	
}

ThreadPool::~ThreadPool() {
	done = true;
	for (auto &thread : threads) thread.join();
}