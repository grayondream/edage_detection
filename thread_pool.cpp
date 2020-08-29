#include "thread_pool.hpp"

/*
* @brief 从队列中获取一个任务
*/
core::thread_pool::task core::thread_pool::get_task()
{
	std::unique_lock<std::mutex> lock(_mutex);
	while (_tasks.empty() && is_running())
	{
		_cond.wait(lock);
	}

	task t;
	if (!_tasks.empty() && is_running())
	{
		t = _tasks.front();
		_tasks.pop();

	}

	if (_tasks.empty())
	{
		_empty_cond.notify_one();
	}

	return t;
}

/*
* @brief 任务循环
*/
void core::thread_pool::loop()
{
	//std::cout << "thread " << std::this_thread::get_id() << " begin to work!" << std::endl;
	while (is_running())
	{
		task t = get_task();
		if (t)
		{
			t();
		}
	}

	//std::cout << "thread " << std::this_thread::get_id() << " going to die!" << std::endl;
}

/*
* @brief 启动线程池
*/
void core::thread_pool::start()
{
	std::mutex m;
	m.lock();
	_running = true;
	for (int i = 0; i < _thread_no; i++)
	{
		_threads.push_back(new std::thread(&thread_pool::loop, this));
	}
	m.unlock();
}

/*
* @brief 关闭线程池
*/
void core::thread_pool::stop()
{
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_running = false;
		_cond.notify_all();
	}

	for (auto it = _threads.begin(); it != _threads.end(); it++)
	{
		if ((*it) != nullptr && (*it)->joinable())
		{
			(*it)->join();
			delete *it;
			*it = nullptr;
		}
	}

	_threads.clear();
}

/*
* @brief 将任务加入到队列中
*/
void core::thread_pool::append_task(const task &t)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_tasks.push(t);
	_cond.notify_one();
}