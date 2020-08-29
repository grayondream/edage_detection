#ifndef __MAIN_TASK_HPP__
#define __MAIN_TASK_HPP__


#include <io.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "png_image.hpp"
#include "filesystem.hpp"
#include "roberts.hpp"
#include "grad_edage.hpp"
#include "sobel_edage.hpp"
#include "thread_pool.hpp"
#include <cstring>
#include "notify.hpp"

class main_detect_task
{
	core::thread_pool pool;
public:
	main_detect_task() {}
	~main_detect_task()
	{

	}

public:

	/*
	* @brief
	* @param	file		需要进行检测的文件名
	* @param	dst_path	目标路径
	* @param	alg_type	采用的算法
	*/
	void edage_detection(string file, string dst_path, core::edage_detection* alg);

	/*
	* @brief	检测
	* @param src_path	原文件目录
	* @param dst_path	目标文件输出目录
	* @param thread_no	线程池的线程数
	* @param alg_type	所使用的边缘检测算子
	*/
	void detect_task(string src_path, string dst_path, const int thread_no, core::edage_detection* alg_type);

	/*
	* @brief 后续清理工作
	*/
	void clear_context()
	{
		pool.wait();
	}
};


#endif //__MAIN_TASK_HPP__