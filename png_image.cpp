#include "png_image.hpp"

#include <cstdio>
#include <fstream>

using std::fstream;
/************************************************************************/
/*                                     png_data                         */
/************************************************************************/
core::png_data::png_data(const png_data &dat)
{
	if (dat._data == nullptr)
		return;

	_data = pixel::pixel_calloc(dat._width, dat._height);
	for (int i = 0; i < dat._height; i++)
	{
		for (int j = 0; j < dat._width; j++)
		{
			_data[i][j] = dat._data[i][j];
		}
	}

	_width = dat._width;
	_height = dat._height;
	_channel_no = dat._channel_no;
	_color_type = dat._color_type;
	_depth = dat._depth;
}


core::png_data core::png_data::operator=(const png_data &dat)
{
	//异常安全，如果内存申请失败，类的构造也会随之失败
	png_data tmp(dat);
	core::pixelpp tmp_data = this->_data;
	this->_data = tmp._data;
	tmp._data = tmp_data;

	this->_channel_no = tmp._channel_no;
	this->_color_type = tmp._color_type;
	this->_depth = tmp._depth;
	this->_height = tmp._height;
	this->_width = tmp._width;

	return *this;
}

core::png_data core::png_data::zero(int w, int h, int type)
{
	png_data ret;
	ret._data = pixel::pixel_calloc(w, h);
	ret._color_type = type;
	switch (type)
	{
	case PNG_COLOR_TYPE_RGBA:
		ret._channel_no = 4; break;
	case PNG_COLOR_TYPE_RGB:
		ret._channel_no = 3; break;
	case PNG_COLOR_TYPE_GRAY:
		ret._channel_no = 1; break;
	}

	ret._width = w;
	ret._height = h;
	ret._depth = PNG_DEPTH;		//TODO:
	return ret;
}

void core::png_data::clear()
{
	if (_data != nullptr)
	{
		for (int i = 0; i < _height; i++)
		{
			delete[] _data[i];
			_data[i] = nullptr;
		}

		delete[] _data;
		_data = nullptr;
	}

	_width = 0;
	_height = 0;
	_color_type = 0;
	_channel_no = 0;
	_depth = 0;
}

/************************************************************************/
/*                                    png_image                         */
/************************************************************************/
/*
* @brief 清理相关描述符
* @param pimg png文件数据信息
* @param pinfo png文件属性信息
* @param fp 打开的文件描述符
*/
void core::png_image::clear_png_ptr(png_structpp pimg, png_infopp pinfo, FILE **fp)
{
	//清理png库相关的指针
	if (nullptr != pinfo)
		png_free_data(*pimg, *pinfo, PNG_FREE_ALL, -1);

	if (nullptr != pimg)
		png_destroy_read_struct(pimg, (png_infopp)NULL, (png_infopp)NULL);

	if (fp)
		fclose(*fp);
}

/*
* @brief 判断文件是否为png文件
* @param name	文件名
* @return true is png, false is not
*/
bool core::png_image::is_png(const char *name)
{
	//通过读取前PNG_HEAD_BYTE_SZ个字符来判断该文件是不是png文件
	fstream is(name, ios::in | ios::binary);
	if (!is.is_open())
		return false;									//文件无法正常打开

	char buff[PNG_HEAD_BYTE_SZ] = { 0 };
	is.read(buff, PNG_HEAD_BYTE_SZ);
	is.close();

	int ret = png_sig_cmp((png_bytep)buff, (png_size_t)0, PNG_HEAD_BYTE_SZ);
	if (ret == 0)										//ret == 0 表示为png文件，ret!=0表示非png文件
		return true;

	return false;										//非png文件
}

/*
* @brief 根据文件名获取文件的元数据
* @param filename 文件名
* @param pimg	元数据
* @param pinfo 图像信息
* @return 错误编号
*/
int core::png_image::get_meta_data(const char *filename, png_structp &pimg, png_infop &pinfo)
{
	if (!empty())
		return PNG_READ_ERROR_NOTEMPTY;			//当前类包含其他数据，加载数据之前需要清空数据

	if (!is_png(filename))
		return PNG_READ_ERROR_INVAILD;			//文件为非法png文件

	FILE *fp = nullptr;
	int err = fopen_s(&fp, filename, "rb");
	if (0 != err)
	{
		return PNG_READ_ERROR_FILE;
	}

	pimg = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!pimg)
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_READ_ERROR_STRUC;
	}

	pinfo = png_create_info_struct(pimg);
	if (!pinfo)
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_READ_ERROR_INFO;
	}

	if (setjmp(png_jmpbuf(pimg)))
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_READ_ERROR_SETJMP;
	}

	//复位文件指针，虽然并未对文件进行操作

	rewind(fp);
	png_init_io(pimg, fp);

	//读取文件的图像信息和像素数据
	png_read_png(pimg, pinfo, PNG_TRANSFORM_EXPAND, 0);
	//获取图像的颜色类型
	_data._color_type = png_get_color_type(pimg, pinfo);

	//获取图像的宽和高
	_data._height = png_get_image_height(pimg, pinfo);
	_data._width = png_get_image_width(pimg, pinfo);
	_data._depth = png_get_bit_depth(pimg, pinfo);
	if (_data._height > PNG_UINT_32_MAX / sizeof(png_byte)
		|| _data._width > PNG_UINT_32_MAX / sizeof(png_byte))
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_READ_ERROR_OVERFLOW;
	}

	clear_png_ptr(nullptr, nullptr, &fp);
	return PNG_READ_SUCCESS;
}

/*
* @brief 根据文件名加载图像的数据
* @param filename 文件名
* @return 加载是否成功的状态编码
*/
int core::png_image::load_from_file(const char *filename)
{
	png_structp pimg = nullptr;
	png_infop pinfo = nullptr;
	int error = get_meta_data(filename, pimg, pinfo);
	if (error != PNG_READ_SUCCESS)
	{
		if (error != PNG_READ_ERROR_FILE
			&& error != PNG_READ_ERROR_INVAILD
			&& error != PNG_READ_ERROR_NOTEMPTY)
			clear_png_ptr(&pimg, &pinfo, nullptr);

		return error;
	}

	//获取数据指针
	png_bytepp prow = png_get_rows(pimg, pinfo);
	_data._data = core::pixel::pixel_calloc(width(), heigth());
	int channel_no = 0;
	switch (_data._color_type)
	{
	case PNG_COLOR_TYPE_RGBA:			//包含透明通道的四通道数据
		_data._channel_no = 4;
		channel_no = _data._channel_no;
		for (int y = 0; y < heigth(); y++)
		{
			for (int x = 0; x < width(); x++)
			{
				_data._data[y][x] = pixel(prow[y][x * channel_no], prow[y][x * channel_no + 1], prow[y][x * channel_no + 2], prow[y][x * channel_no + 3]);
			}
		}
		break;
	case PNG_COLOR_TYPE_RGB:				//不包含透明通道的三通道数据
		_data._channel_no = 3;
		channel_no = _data._channel_no;
		for (int y = 0; y < heigth(); y++)
		{
			for (int x = 0; x < width(); x++)
			{
				_data._data[y][x] = pixel(prow[y][x * channel_no], prow[y][x * channel_no + 1], prow[y][x * channel_no + 2], 0);
			}
		}
		break;
	default:
		_data._channel_no = 0;
		return PNG_WRITE_ERROR_UNKOWN;
	}

	clear_png_ptr(&pimg, &pinfo, nullptr);		//清理工作
	return PNG_READ_SUCCESS;
}

/*
* @brief 将当前png类写入文件
* @param filename 写入路径
* @param txt 文件描述信息
*/
int core::png_image::write2file(const char *filename, const char *txt)
{
	if (empty())
		return PNG_WRITE_ERROR_EMPTY;

	FILE *fp = nullptr;
	int err = fopen_s(&fp, filename, "wb");
	if (0 != err)
	{
		return PNG_WRITE_ERROR_FILE;
	}

	//创建相关的结构体
	png_structp pimg = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop pinfo = nullptr;
	if (nullptr == pimg)
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_WRITE_ERROR_STRUC;
	}

	pinfo = png_create_info_struct(pimg);
	if (pinfo == nullptr)
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_WRITE_ERROR_INFO;
	}

	if (setjmp(png_jmpbuf(pimg)))
	{
		clear_png_ptr(&pimg, &pinfo, &fp);
		return PNG_WRITE_ERROR_SETJMP;
	}

	png_init_io(pimg, fp);
	png_set_IHDR(pimg, pinfo, _data._width, _data._height, _data._depth, _data._color_type,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	//写入描述信息
	if (nullptr != txt)
	{
		png_text title;
		title.compression = PNG_TEXT_COMPRESSION_NONE;
		title.key = "Title";
		title.text = (png_charp)txt;
		png_set_text(pimg, pinfo, &title, 1);
	}

	//将元数据写入具体的结构体中
	png_write_info(pimg, pinfo);
	int channel_no = _data._channel_no;
	png_bytep prow = new png_byte[channel_no * width() * sizeof(png_byte)];
	for (int y = 0; y < heigth(); y++)
	{
		for (int x = 0; x < width(); x++)
		{
			switch (_data._color_type)
			{
			case PNG_COLOR_TYPE_RGBA:
				prow[x * channel_no + 3] = _data._data[y][x].a;
				//go through
			case PNG_COLOR_TYPE_RGB:
				prow[x * channel_no] = _data._data[y][x].r;
				prow[x * channel_no + 1] = _data._data[y][x].g;
				prow[x * channel_no + 2] = _data._data[y][x].b;
				break;
			case PNG_COLOR_TYPE_GRAY:
				prow[x * channel_no] = _data._data[y][x].r;
				break;
			default:
				return PNG_WRITE_ERROR_UNKOWN;
			}
		}

		png_write_row(pimg, prow);
	}

	//清理工作
	png_write_end(pimg, nullptr);
	clear_png_ptr(&pimg, &pinfo, &fp);
	return PNG_WRITE_SUCCESS;
}