#ifndef __EDAGE_DETECTION_HPP__
#define __EDAGE_DETECTION_HPP__

#include "png_image.hpp"
#include "thread_pool.hpp"
#define EDAGE_DETECTION_TYPE_GRAD 1
#define EDAGE_DETECTION_TYPE_ROBERTS 2
#define EDAGE_DETECTION_TYPE_SOBEL 3
namespace core
{
	/*
	 * @brief 边缘检测算法基类
	 */
	class edage_detection
	{
	protected:
		int _thread_no = 0;
	public:
		virtual ~edage_detection() {};
		virtual void detect(core::png_image &src, core::png_image &dst) = 0;
		virtual string to_string() = 0;
		void set_threadno(int threadno)
		{
			_thread_no = threadno;
		}
		
	};
}


#endif //__EDAGE_DETECTION_HPP__