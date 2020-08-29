#include <iostream>
#include "getopt.h"
#include <string>
#include "edage_detection.hpp"
#include "roberts.hpp"
#include "grad_edage.hpp"
#include "sobel_edage.hpp"
#include "filesystem.hpp"
#include "notify.hpp"
#include <ctime>
#include <thread>
#include "main_detect_task.hpp"

using std::string;
using std::cin;

/*
 * @brief command line 
 */
void show_usage()
{
	char *help = "边缘检测使用规范:\n"
		"\t--arithmetic\t\t选用边缘检测算法算子，可选参数为[robert|sobel|grad]\n"
		"\t--scale\t\t\t设置算法参数scale，取值为浮点数\n"
		"\t--main_thread_no\t主进程线程池初始化线程数，取值为int\n"
		"\t--thread_no\t\t算法使用线程池初始化线程数，取值为int\n"
		"\t--in\t\t\t文件输入目录\n"
		"\t--out\t\t\t文件输出目录，不建议和输入目录相同（写文件会覆盖原文件）\n"
		"\t--help\t\t\t显示帮助信息\n"
		"\t\t\t\t\t\t\t\t\t\tby grayondream\n";
	core::notify::get_instance()->info(string(help));
}

/*
 * @brief
 * @param argc	参数量
 * @param argv	具体参数值
 */
void command_opts(int argc, char **argv)
{
	struct option long_opts[] = {
		{"arithmetic", required_argument, NULL, 'a'},				//算子
		{ "scale", required_argument, NULL, 's' },					//比例
		{ "main_thread_no", required_argument, NULL, 'm' },			//主线程线程数
		{ "thread_no", required_argument, NULL, 't' },				//算法线程数
		{ "in", required_argument, NULL, 'i' },						//输入目录
		{ "out", required_argument, NULL, 'o' },					//输出目录
		{ "help", required_argument, NULL, 'h' },					//帮助
	};

	char *short_opt = "a:s:t:i:o:h";
	int opt_idx = 0;
	char ch = 0;
	
	string src;
	string dst;
	int thread_no = 5;
	int main_thread_no = 5;
	core::edage_detection* alg = nullptr;
	double scale = 0.0;
	while (ch = getopt_long(argc, argv, short_opt, long_opts, &opt_idx))		//参数解析
	{
		if (ch == -1)
			break;

		switch (ch)
		{
		case 'a':
			{
				string arch(optarg);
				if (arch == string("robert"))
				{
					alg = new core::roberts_detection;
				}
				else if (arch == string("grad"))
				{
					alg = new core::grad_detection;
				}
				else if (arch == string("sobel"))
				{
					alg = new core::sobel_detection;
				}
			}
			break;
		case 's':
			scale = atof(optarg);
			break;
		case 't':
			thread_no = atoi(optarg);
			break;
		case 'm':
			main_thread_no = atoi(optarg);
			break;
		case 'i':
			src = string(optarg);
			break;
		case 'o':
			dst = string(optarg);
			break;
		case 'h':
		default:
			show_usage();
			return;
		}
	}
	
	//参数检查
	if (alg == nullptr || src.empty() || dst.empty())
	{
		string error_log;
		if (alg == nullptr)
		{
			error_log += string("[边缘检测算子]");
		}

		if (src.empty())
		{
			error_log += string("[源目录]");
		}

		if (dst.empty())
		{
			error_log += string("[输出目录]");
		}

		if(!error_log.empty())
			error_log = string("\t请设置") + error_log + string("\n");
		core::notify::get_instance()->error(error_log);

		show_usage();
		return;
	}

	//设置算子的参数
	alg->set_threadno(thread_no);
	if (dynamic_cast<core::roberts_detection*>(alg) == alg)
	{
		dynamic_cast<core::roberts_detection*>(alg)->set_scale(scale);
	}
	else if (dynamic_cast<core::grad_detection*>(alg) == alg)
	{
		dynamic_cast<core::grad_detection*>(alg)->set_scale(scale);
	}
	else if (dynamic_cast<core::sobel_detection*>(alg) == alg)
	{
		dynamic_cast<core::sobel_detection*>(alg)->set_scale(scale);
	}

	src = core::filesystem::convert_path(src.c_str());
	dst = core::filesystem::convert_path(dst.c_str());

	main_detect_task main_task;
	main_task.detect_task(src, dst, main_thread_no, alg);
	main_task.clear_context();
	//getchar();
	//清理
	if (alg != nullptr)
	{
		delete alg;
		alg = nullptr;
	}
}

void gui_show()
{}

int main(int argc, char **argv)
{
	//filesystem_test();
	//thread_pool_test();
	double start = clock();
	core::windows_stream stream;
	core::notify::get_instance()->set_stream(&stream);
	command_opts(argc, argv);
	
	double cost_time = (clock() - start) / CLOCKS_PER_SEC;
	core::notify::get_instance()->warn(string("程序运行花费时间") + std::to_string(cost_time) + string("s\n"));
	//_CrtDumpMemoryLeaks();	内存泄漏检测
	
	return 0;
}