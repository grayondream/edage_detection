#include "sobel_edage.hpp"
#include <cmath>


void core::sobel_detection::detect_row(core::png_image &src, core::png_image &dst, int y)
{
	for (int x = 1; x < src.width() - 1; x++)
	{
		core::pixel cur(src[y][x]);

		core::pixel cur_lt(src[y - 1][x - 1]);
		core::pixel cur_top(src[y - 1][x]);
		core::pixel cur_right(src[y][x + 1]);

		core::pixel cur_left(src[y][x - 1]);
		core::pixel cur_ld(src[y + 1][x - 1]);
		core::pixel cur_down(src[y + 1][x]);
		core::pixel cur_rd(src[y + 1][x + 1]);

		int rgb_rst = abs(cur_lt.r + cur_top.r * 2 + cur_right.r - cur_ld.r - cur_down.r * 2 - cur_rd.r)
			+ abs(cur_lt.g + cur_top.g * 2 + cur_right.g - cur_ld.g - cur_down.g * 2 - cur_rd.g)
			+ abs(cur_lt.b + cur_top.b * 2 + cur_right.b - cur_ld.b - cur_down.b * 2 - cur_rd.b);

		int rgb_snd = abs(cur_lt.r + cur_left.r * 2 + cur_ld.r - cur_right.r - cur_right.r * 2 - cur_rd.r)
			+ abs(cur_lt.g + cur_left.g * 2 + cur_ld.g - cur_right.g - cur_right.g * 2 - cur_rd.g)
			+ abs(cur_lt.b + cur_left.b * 2 + cur_ld.b - cur_right.b - cur_right.b * 2 - cur_rd.b);

		int value = 0;
		if (type == 0)
		{
			if (rgb_rst > rgb_snd)
				value = rgb_rst;
			else
				value = rgb_snd;
		}
		else if (type == 1)
		{
			value = (rgb_rst + rgb_snd) / 2;
		}

		value = value * scale;
		value = value > 255 ? 255 : value;
		dst[y][x] = core::pixel(value, value, value, 0);
	}
}

void core::sobel_detection::detect(core::png_image &src, core::png_image &dst)
{
	if (src.empty())
		return;

	core::thread_pool pool;
	pool.init(_thread_no);
	dst = core::png_image::zero(src.width(), src.heigth(), PNG_COLOR_TYPE_GRAY);
	for (int y = 1; y < src.heigth() - 1; y++)
	{
		pool.append_task([this, &src, &dst, y]() {this->detect_row(src, dst, y); });
		//detect_row(src, dst, y);
	}

	pool.wait();
}

string core::sobel_detection::to_string()
{
	return string("sobel_") + std::to_string(int(100 * scale));
}