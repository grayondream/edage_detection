#include "filesystem.hpp"

#include <io.h>
#include <windows.h>
#include <direct.h>


/*
* @brief 判断路径是不是一个文件夹
* @param path 路径
*/
bool core::filesystem::is_directory(const char *path)
{
	if (_access(path, 0) == -1)
		return false;

	DWORD ret = GetFileAttributesA(path);
	return ret == FILE_ATTRIBUTE_DIRECTORY;
}

/*
* @brief 判断路径是不是一个文件
* @param path 文件路径
*/
bool core::filesystem::is_file(const char *filename)
{
	if (_access(filename, 0) == -1)
		return false;

	DWORD ret = GetFileAttributesA(filename);
	return ret != FILE_ATTRIBUTE_DIRECTORY;
}

/*
* @brief 根据路径获取文件所在目录
* @param path 文件路径
*/
string core::filesystem::get_directory(const char *filename)
{
	string file = convert_path(filename);
	int offset = file.find_last_of('/');
	return string(file.begin(), file.begin() + offset + 1);
}

/*
* @brief 根据文件路径获取不包含路径的文件名
* @param filename	文件路径
*/
string core::filesystem::get_filename(const char *filename)
{
	string file = convert_path(filename);
	int offset = file.find_last_of('/');
	return string(file.begin() + 1 + offset, file.end());
}

/*
* @brief 递归创建文件夹
* @param path 路径
*/
int core::filesystem::create_directory(const char *path)
{
	if (!core::filesystem::is_directory(path))
	{
		return _mkdir(path);
	}

	return 0;
}

/*
* @brief 将路径名转换成统一的格式，比如将C:\\A\\B和C:\A\B转换成C:/A/B，不支持转义
* @param path 路径名
*/
string core::filesystem::convert_path(const char *path)
{
	string ret(path);
	//先将\\替换为//
	for (int i = 0; i < ret.size(); i++)
	{
		if (ret[i] == '\\')
			ret[i] = '/';
	}

	return ret;
}

/*
* @brief 根据文件扩展名获取当前目录中的对应文件
* @param path 目录
* @param ext  目标文件的扩展名
*/
vector<string> core::filesystem::get_files(const char *path, int type)
{
	if (false == filesystem::is_directory(path))
		return vector<string>();

	string filter_path(path);
	if (filter_path[filter_path.size() - 1] != '/')
		filter_path += string(1, '/');

	switch (type)
	{
	case FILE_PNG:
		filter_path += string("*.png"); break;
	default:
		return vector<string>();
	}

	struct _finddata_t fileinfo;
	long handle = _findfirst(filter_path.c_str(), &fileinfo);
	if (-1 == handle)
		return vector<string>();

	vector<string> pngs;
	do
	{
		string filename = string(path) + string("/") + string(fileinfo.name);
		pngs.push_back(filename);
	} while (!_findnext(handle, &fileinfo));

	_findclose(handle);
	return pngs;
}

/*
* @brief 将第二个路径路径合并到第一个路径后面
* @param src	前端目录
* @param dst   后端目录
*/
string core::filesystem::path_append(const string &src, const string &dst)
{
	string rst = convert_path(src.c_str());
	string snd = convert_path(dst.c_str());
	if (rst[rst.size() - 1] != '/')
		rst += string("/");

	if (snd[0] == '/')
		snd = string(snd.begin() + 1, snd.end());

	return rst + snd;
}