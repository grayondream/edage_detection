#include "roberts.hpp"

void core::roberts_detection::detect_row(core::png_image &src, core::png_image &dst, int y)
{
	for (int x = 0; x < src.width() - 1; x++)
	{
		core::pixel cur(src[y][x]);
		core::pixel cur_down(src[y + 1][x]);
		core::pixel cur_right(src[y][x + 1]);
		core::pixel cur_rd(src[y + 1][x + 1]);

		core::pixel tmp_rst = cur - cur_rd;
		core::pixel tmp_snd = cur_right - cur_down;

		int value = (tmp_rst.r + tmp_rst.g + tmp_rst.b) + (tmp_snd.r + tmp_snd.g + tmp_snd.b);
		value = value * scale;
		value = value > 255 ? 255 : value;
		dst[y][x] = core::pixel(value, value, value, 0);
	}
}

void core::roberts_detection::detect(core::png_image &src, core::png_image &dst)
{
	if (src.empty())
		return;

	core::thread_pool pool;
	pool.init(_thread_no);
	dst = core::png_image::zero(src.width(), src.heigth(), PNG_COLOR_TYPE_GRAY);
	for (int y = 0; y < src.heigth() - 1; y++)
	{
		pool.append_task([this, &src, &dst, y]() {this->detect_row(src, dst, y); });
		//detect_row(src, dst, y);
	}

	pool.wait();
}

string core::roberts_detection::to_string()
{
	return string("roberts_") + std::to_string(int(100 * scale));
}