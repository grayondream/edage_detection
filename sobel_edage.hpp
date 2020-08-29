/*
 * @brief sobel边缘检测
 */
#ifndef __SOBEL_EDAGE_HPP__
#define __SOBEL_EDAGE_HPP__

#include "edage_detection.hpp"


namespace core
{
	/*
	* @brief sobel边缘检测实现
	*/
	class sobel_detection : public edage_detection
	{
		double scale;
		int type;
	public:
		sobel_detection() 
		{
			scale = 1.0;
			type = 1;
		}
		virtual ~sobel_detection() {}
		void set_scale(double sc, int ty=1)
		{
			scale = sc;
			type = ty;
		}
		void core::sobel_detection::detect_row(core::png_image &src, core::png_image &dst, int y);
		virtual void detect(core::png_image &src, core::png_image &dst);
		virtual string to_string();
	};
}

#endif //__SOBEL_EDAGE_HPP__