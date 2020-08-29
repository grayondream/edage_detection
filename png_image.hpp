
/*
 * @brief png文件信息相关内容
 */
#ifndef __PNG_IMAGE_HPP__
#define __PNG_IMAGE_HPP__

#include <string>

using std::string;
using std::ios;
#include <png.h>
#ifdef DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace core
{

#define PNG_HEAD_BYTE_SZ 8

//读取信息相关状态码
#define PNG_READ_SUCCESS 0				//读取成功
#define PNG_READ_ERROR_NOTEMPTY 1		//当前数据不为空
#define PNG_READ_ERROR_FILE 2			//文件读取失败
#define PNG_READ_ERROR_STRUC 3			//创建PNG STRUCTURE失败
#define PNG_READ_ERROR_INFO 4			//创建PNG INFO失败
#define PNG_READ_ERROR_INVAILD 5		//文件为非法PNG文件
#define PNG_READ_ERROR_SETJMP 6			//设置JMP失败
#define PNG_READ_ERROR_OVERFLOW 7		//图片过大，当前机器无法处理
#define PNG_READ_ERROR_ROW 8			//读取图像元数据失败
#define PNG_READ_ERROR_META 9			//获取图像元数据失败

//写信息相关状态码
#define PNG_WRITE_SUCCESS 0				//数据写入成功
#define PNG_WRITE_ERROR_EMPTY 1			//当前数据为空无法写入
#define PNG_WRITE_ERROR_FILE 2			//创建文件失败
#define PNG_WRITE_ERROR_STRUC 3			//创建png structure失败
#define PNG_WRITE_ERROR_INFO 4			//创建png info失败
#define PNG_WRITE_ERROR_SETJMP 5		//设置jmp失败
#define PNG_WRITE_ERROR_ROW	 6			//写入图像元数据失败
#define PNG_WRITE_ERROR_UNKOWN 7 

//不同通道信息
#define PNG_RGBA_CHANNEL 4			
#define PNG_RGB_CHANNEL 3
#define PNG_GRAY_CHANNEL 1

//不同图像对应的深度
#define PNG_DEPTH 8

	class pixel;
	typedef pixel* pixelp;
	typedef pixel**pixelpp;
	/*
	 * @brief 每个像素的元数据，需要配合png_data使用，如果pixel为灰度图则所有的rgb值相同;如果为RGB三通道则A值无效
	 * @param r,g,b,a 分别表示图像的rgba值
	 */
	class pixel
	{
	public:
		png_byte r;
		png_byte g;
		png_byte b;
		png_byte a;
	public:
		pixel() {}
		pixel(png_byte rr, png_byte gg, png_byte bb, png_byte aa)
			:r(rr), g(gg), b(bb), a(aa) 
		{}

	public:
		inline static pixelpp pixel_calloc(int w, int h)
		{
			pixelpp data = new pixelp[h];
			for (int i = 0; i < h; i++)
			{
				data[i] = new pixel[w];
				memset(data[i], 0, sizeof(pixel) * w);
			}

			return data;
		}

		inline pixel operator-(const pixel &p)
		{
			return pixel(r - p.r, g - p.g, b - p.b, a - p.a);
		}
	};

	/*
	 * @brief 图像数据中的具体信息
	 * @param _width,_height 尺寸，宽长
	 * @param _data	元数据
	 * @param _color_type 继承自libpng
	 * @param _channel_no 通道数，可取1，3，4
	 */
	struct png_data
	{
		int _width;
		int _height;
		core::pixelpp _data;			//二维数据，基本格式按照rgba通道存储
		int _color_type;
		int _channel_no;
		int _depth;						//位深

		png_data()
		{
			_width = 0;
			_height = 0;
			_data = nullptr;
			_color_type = 0;
			_channel_no = 0;
			_depth = 0;
		}

		png_data(const png_data &dat);
		png_data operator=(const png_data &dat);
		static png_data zero(int w, int h, int type);

		~png_data()
		{
			clear();
		}

		void clear();
	};

	/*
	 * @brief 简单的png文件结构，包含基本的文件信息
	 */
	class png_image
	{
	private:
		png_data _data;

	public:
		png_image()
		{
		}

		~png_image()
		{

		}
	private:
		/*
		 * @brief 清理相关描述符
		 * @param pimg png文件数据信息
		 * @param pinfo png文件属性信息
		 * @param fp 打开的文件描述符
		 */
		void clear_png_ptr(png_structpp pimg, png_infopp pinfo, FILE **fp);

	public:
		inline static png_image zero(int w, int h, int type)
		{
			png_image ret;
			ret._data = png_data::zero(w, h, type);
			return ret;
		}

		//判断当前类中是否包含数据
		inline bool empty()
		{
			return nullptr == _data._data;
		}

		inline int width() const
		{
			return _data._width;
		}

		inline int heigth() const
		{
			return _data._height;
		}

		inline int color_type() const
		{
			return _data._color_type;
		}

		inline png_data get_data() const
		{
			return _data;
		}

		inline pixelp operator[](int idx)
		{
			return _data._data[idx];
		}

		/*
		 * @brief 判断文件是否为png文件
		 * @param name	文件名
		 * @return true is png, false is not
		 */
		static bool is_png(const char *name);

		void load_from_data(core::pixelpp dat, int w, int h, int type, int ch_no)
		{
			_data._data = dat;
			_data._width = w;
			_data._height = h;
			_data._color_type = type;
			_data._channel_no = ch_no;
		}

		/*
		 * @brief 根据文件名获取文件的元数据
		 * @param filename 文件名
		 * @param pimg	元数据
		 * @param pinfo 图像信息
		 * @return 错误编号
		 */
		int get_meta_data(const char *filename, png_structp &pimg, png_infop &pinfo);

		/*
		 * @brief 根据文件名加载图像的数据
		 * @param filename 文件名
		 * @return 加载是否成功的状态编码
		 */
		int load_from_file(const char *filename);

		/*
		 * @brief 将当前png类写入文件
		 * @param filename 写入路径
		 * @param txt 文件描述信息
		 */
		int write2file(const char *filename, const char *txt = nullptr);
	};
}


#endif //__PNG_IMAGE_HPP__
