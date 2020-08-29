#include "main_detect_task.hpp"

/*
* @brief
* @param	file		需要进行检测的文件名
* @param	dst_path	目标路径
* @param	alg_type	采用的算法
*/
void main_detect_task::edage_detection(string file,string dst_path, core::edage_detection* alg)
{
	file = core::filesystem::convert_path(file.c_str());
	dst_path = core::filesystem::convert_path(dst_path.c_str());
	core::png_image img;
	int ret = img.load_from_file(file.c_str());
	if (PNG_READ_SUCCESS != ret)
	{
		core::notify::get_instance()->error(string("读取文件") + file + string("失败，错误码") + std::to_string(ret) + string("\n"));
		return;
	}

	core::png_image dst_img;
	alg->detect(img, dst_img);
	string filename = core::filesystem::get_filename(file.c_str());
	filename = alg->to_string() + filename;
	filename = core::filesystem::path_append(dst_path, filename);
	ret = dst_img.write2file(filename.c_str());
	if (ret != PNG_WRITE_SUCCESS)
	{
		core::notify::get_instance()->error(string("写入文件") + filename + string("失败，错误码") + std::to_string(ret) + string("\n"));
		return;
	}

	core::notify::get_instance()->info(string(string("处理文件") + file + "完成,生成的文件存储为" + filename + string("\n")));
}

/*
* @brief	检测
* @param src_path	原文件目录
* @param dst_path	目标文件输出目录
* @param thread_no	线程池的线程数
* @param alg    	所使用的边缘检测算子
*/
void main_detect_task::detect_task(string src_path, string dst_path, const int thread_no, core::edage_detection* alg)
{
	if (!core::filesystem::is_directory(src_path.c_str()))
	{
		core::notify::get_instance()->error(string(src_path + string("并不是一个有效的目录\n")));
		return;
	}

	int ret = core::filesystem::create_directory(dst_path.c_str());
	if (-1 == ret)
	{
		core::notify::get_instance()->error(string(string("创建目录") + src_path + string("失败\n")));
		return;
	}

	vector<string> files = core::filesystem::get_files(src_path.c_str(), FILE_PNG);
	core::notify::get_instance()->info(string("在目标目录中检测到") + std::to_string(files.size()) + string("个文件\n"));

	pool.init(thread_no);
	for (string file : files)
	{
		pool.append_task([this, file, dst_path, alg]() {this->edage_detection(file, dst_path, alg); });
	}
}
