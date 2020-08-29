#pragma once
#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include <queue>
#include <atomic>
#include <condition_variable>

namespace core
{
	/*
	 * @brief 线程池实现
	 */
	class thread_pool
	{
	private:
		typedef std::function<void()> task;

		std::atomic_bool _running;				//显示线程池的状态
		std::mutex _mutex;
		std::condition_variable _cond;
		std::mutex _empty_mutex;
		std::condition_variable _empty_cond;
		std::atomic<int> _thread_no;

		std::vector<std::thread*> _threads;
		std::queue<task> _tasks;
	private:
		thread_pool(const thread_pool &tp) = delete;
		thread_pool &operator=(const thread_pool &tp) = delete;

		

	public:
		thread_pool()
			: _thread_no(0),
			_running(false),
			_cond(),
			_mutex(),
			_empty_cond(),
			_empty_mutex()
		{
			
		}

		void init(int no)
		{
			_thread_no = no;
			start();
		}

		/*
		 * @brief 等待线程中的任务全部执行结束
		 */
		inline void wait()
		{
			std::unique_lock<std::mutex> lock(_empty_mutex);
			while (!_tasks.empty())
			{
				_empty_cond.wait(lock);
			}

			if(is_running())
				stop();
		}

		~thread_pool()
		{
			
		}

		inline bool is_running()
		{
			return _running;
		}

		inline bool empty()
		{
			return _tasks.empty();
		}

	private:
		/*
		 * @brief 从队列中获取一个任务
		 */
		thread_pool::task get_task();

		/*
		 * @brief 任务循环
		 */
		void loop();

		/*
		 * @brief 启动线程池
		 */
		void start();

	public:
		/*
		 * @brief 关闭线程池
		 */
		void stop();

		/*
		 * @brief 将任务加入到队列中
		 */
		void append_task(const task &t);
	};
}


#endif //__THREAD_POOL_HPP__