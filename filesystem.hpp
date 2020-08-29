/*
 * @brief 文件操作，文件夹操作等相关操作，当前代码平台相关
 */
#ifndef __FILESYSTEM_HPP__
#define __FILESYSTEM_HPP__

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace core
{
#define FILE_PNG 0
#define FILE_EXE 1
	/*
	* @brief 文件操作，文件夹操作等相关操作，当前部分平台相关
	* @note	 可能需要添加平台无关的代码
	*/
	class filesystem
	{
	public:
		/*
		 * @brief 判断文件夹是不是一个文件夹
		 * @param	path	路径
		 */
		static bool is_directory(const char *path);

		/*
		 * @brief 判断路径是不是一个文件
		 * @param path 文件路径
		 */
		static bool is_file(const char *filename);

		/*
		 * @brief 根据路径获取文件所在目录
		 * @param path 文件路径
		 */
		static string get_directory(const char *filename);

		/*
		 * @brief 根据文件路径获取不包含路径的文件名
		 * @param filename	文件路径
		 */
		static string get_filename(const char *filename);

		/*
		 * @brief 递归创建文件夹
		 * @param path 路径
		 */
		static int create_directory(const char *path);

		/*
		 * @brief 将路径名转换成统一的格式，比如将C:\\A\\B和C:\A\B转换成C:/A/B，不支持转义
		 * @param path 路径名
		 */
		static string convert_path(const char *path);

		/*
		 * @brief 根据文件扩展名获取当前目录中的对应文件
		 * @param path 目录
		 * @param ext  目标文件的扩展名
		 */
		static vector<string> get_files(const char *path, int type);

		/*
		 * @brief 将第二个路径路径合并到第一个路径后面
		 * @param src	前端目录
		 * @param dst   后端目录
		 */
		static string path_append(const string &src, const string &dst);
	};
}

#ifdef TEST
#include <iostream>
using namespace std;

void filesystem_test()
{
	cout << "路径转换测试" << endl;
	string path("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img");
	cout << path << "\t" << core::filesystem::convert_path(path.c_str()) << endl;
	path = string("X:/A/B/C");
	cout << path << "\t" << core::filesystem::convert_path(path.c_str()) << endl;
	path = string("X:\\A\\B\\C\\");
	cout << path << "\t" << core::filesystem::convert_path(path.c_str()) << endl;
	
	cout << "文件存在与否测试" << endl;
	string filename("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\1.png");			//存在
	cout << filename << "\t" << core::filesystem::is_file(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\111.png");		//实际不存在
	cout << filename << "\t" << core::filesystem::is_file(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\");		
	cout << filename << "\t" << core::filesystem::is_file(filename.c_str()) << endl;

	cout << "文件夹测试" << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\");
	cout << filename << "\t" << core::filesystem::is_directory(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\112");
	cout << filename << "\t" << core::filesystem::is_directory(filename.c_str()) << endl;

	cout << "文件夹创建测试" << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\");
	cout << filename << "\t" << core::filesystem::create_directory(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\111");
	cout << filename << "\t" << core::filesystem::create_directory(filename.c_str()) << endl;
	
	cout << "文件夹名称分离测试" << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\1.png");
	cout << filename << "\t" << core::filesystem::get_directory(filename.c_str()) << "\t" << core::filesystem::get_filename(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\");
	cout << filename << "\t" << core::filesystem::get_directory(filename.c_str()) << "\t" << core::filesystem::get_filename(filename.c_str()) << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img");
	cout << filename << "\t" << core::filesystem::get_directory(filename.c_str()) << "\t" << core::filesystem::get_filename(filename.c_str()) << endl;

	cout << "遍历目录测试" << endl;
	filename = string("X:\\Users\\grayondream\\Desktop\\tmp\\mihuyo\\img\\");
	vector<string> ret = core::filesystem::get_files(filename.c_str(), FILE_PNG);
	for (string str : ret)
	{
		cout << str << endl;
	}
}

#endif

#endif //__FILESYSTEM_HPP__