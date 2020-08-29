#ifndef __NOTIFY_HPP__
#define __NOTIFY_HPP__
#include <cstdio>
#include <string>
#include <mutex>
using std::string;

#ifdef _WIN32
#include <windows.h>
#endif

namespace core
{
#define NOTIFY_STREAM_BLUE FOREGROUND_BLUE
#define NOTIFY_STREAM_GREEN FOREGROUND_GREEN
#define NOTIFY_STREAM_RED FOREGROUND_RED
#define NOTIFY_STREAM_NONE 0

	/*
	 * @brief 平台相关的输出流
	 */
	class platform_stream
	{
	protected:
		
	public:
		virtual void set_font_color(int color) = 0;
		virtual void print(const string &str) = 0;
	};

	/*
	 * @brief windows平台输出流实现
	 */
	class windows_stream : public platform_stream
	{
	public:
		void set_font_color(int color) 
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | color);
		}

		void print(const string &str) 
		{
			fflush(stdout);
			const char * line = str.c_str();
			printf(line);					
			fflush(stdout);
		}
	};

	/*
	 * @brief io信息交互类
	 */
	class notify
	{
		std::mutex _mutex;
	private:
		notify() {};
		notify(const notify &s) {};
		notify &operator=(const notify &s) {};

		static notify *_instance;
		platform_stream *_stream;
	public:
		inline static notify *get_instance()
		{
			return _instance;
		}

		inline void set_stream(platform_stream *stream)
		{
			_stream = stream;
		}

		void print(const string &str, int color)
		{
			if (_stream != nullptr)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_stream->set_font_color(color);
				_stream->print(str);
				_stream->set_font_color(NOTIFY_STREAM_NONE);
			}
		}

		inline void info(const string &str)
		{
			print(str, NOTIFY_STREAM_RED | NOTIFY_STREAM_BLUE | NOTIFY_STREAM_GREEN);
		}

		inline void warn(const string &str)
		{
			print(str, NOTIFY_STREAM_GREEN);
		}

		inline void error(const string &str)
		{
			print(str, NOTIFY_STREAM_RED);
		}

		~notify()
		{
		}
	};

}
#endif //__NOTIFY_HPP__