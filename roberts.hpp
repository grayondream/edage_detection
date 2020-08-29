#ifndef __ROBERTS_HPP__
#define __ROBERTS_HPP__

#include "edage_detection.hpp"

namespace core
{
	/*
	 * @brief roberts算子边缘检测算法
	 */
	class roberts_detection : public edage_detection
	{
		double scale;
	public:
		roberts_detection()
		{
			scale = 0.5;
		}

		virtual ~roberts_detection() {}
		void set_scale(double sc)
		{
			scale = sc;
		}

		void detect_row(core::png_image &src, core::png_image &dst, int y);
		/*
		 * @brief	检测
		 * @param src	原图像
		 * @param dst	输出图像
		 */
		virtual void detect(core::png_image &src, core::png_image &dst);

		virtual string to_string();
	};
}

#endif //__ROBERTS_HPP__