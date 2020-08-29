/*
 * @brief 梯度边缘检测实现
 */
#ifndef __GRAD_EDAGE_HPP__
#define __GRAD_EDAGE_HPP__

#include "edage_detection.hpp"

namespace core
{
	/*
	* @brief 梯度边缘检测实现
	*/
	class grad_detection : public edage_detection
	{
	private:
		double scale;

	private:
		void detect_row(core::png_image &src, core::png_image &dst, int y);
	public:
		grad_detection()
		{
			scale = 0.5;
		}

		virtual ~grad_detection(){}

		void set_scale(double sc)
		{
			scale = sc;
		}

		virtual void detect(core::png_image &src, core::png_image &dst);
		virtual string to_string();
	};
}

#endif//__GRAD_EDAGE_HPP__