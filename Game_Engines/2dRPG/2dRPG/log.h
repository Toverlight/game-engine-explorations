#ifndef _LOG_H_
#define _LOG_H_

#include <ctime>
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Windows.h>

enum class LogLevel
{
	Info,
	Warn,
	Error,
	Debug,
	Trace
};
enum class LogTarget
{
	None,
	Console,
	File
};
// 设置控制台文本前景色
inline void setConsoleColor(WORD color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

// 辅助函数：递归终止条件
inline void format_message(std::ostringstream& oss) {}

// 辅助函数：递归展开模板参数包
template<typename T, typename... Args>
void format_message(std::ostringstream& oss, const T& first, const Args&... args) {
	oss << first;  // 处理第一个参数
	format_message(oss, args...);
}

// 主函数：格式化时间和消息
template<typename... Args>
std::ostringstream format_with_time(LogLevel logLevel, const Args&... args) {
	// 获取当前时间
	time_t now = time(nullptr);
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);

	// 格式化时间
	char time_buffer[50];
	strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

	// 构建消息字符串
	std::ostringstream oss;
	oss << "[" << time_buffer << "] ";
	switch (logLevel)
	{
	case LogLevel::Info:
		oss << "Info:";
		break;
	case LogLevel::Warn:
		oss << "Warn:";
		break;
	case LogLevel::Error:
		oss << "Error:";
		break;
	case LogLevel::Debug:
		oss << "Debug:";
		break;
	case LogLevel::Trace:
		oss << "Trace:";
		break;

	}
	format_message(oss, args...);  // 将可变参数格式化

	return oss;
}


class Log
{
public:
	static Log* getInstance();

	void setLevel(LogLevel level) { this->level = level; }
	LogLevel getLevel() { return level; }

	void setTarget(LogTarget target) { this->target = target; }
	LogTarget getTarget() { return target; }

	template<typename... Args>
	void info(const Args&... args)
	{
		std::ostringstream oss = format_with_time(LogLevel::Info, args...);

		switch (target)
		{
		case LogTarget::None:
			break;
		case LogTarget::Console:
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << oss.str() << std::endl;
			break;
		case LogTarget::File:
			if (log_file.is_open())
				log_file << oss.str() << std::endl;
			else
			{
				setConsoleColor(FOREGROUND_RED);
				std::cerr << "WritingError: Log file is not open for writing." << std::endl;
			}
			break;
		}
	}
	template<typename... Args>
	void warn(const Args&... args)
	{
		std::ostringstream oss = format_with_time(LogLevel::Warn, args...);

		switch (target)
		{
		case LogTarget::None:
			break;
		case LogTarget::Console:
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN);
			std::cout << oss.str() << std::endl;
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case LogTarget::File:
			if (log_file.is_open())
				log_file << oss.str() << std::endl;
			else
			{
				setConsoleColor(FOREGROUND_RED);
				std::cerr << "WritingError: Log file is not open for writing." << std::endl;
			}
			break;
		}
	}
	template<typename... Args>
	void error(const Args&... args)
	{
		std::ostringstream oss = format_with_time(LogLevel::Error, args...);

		switch (target)
		{
		case LogTarget::None:
			break;
		case LogTarget::Console:
			setConsoleColor(FOREGROUND_RED);
			std::cout << oss.str() << std::endl;
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case LogTarget::File:
			if (log_file.is_open())
				log_file << oss.str() << std::endl;
			else
			{
				setConsoleColor(FOREGROUND_RED);
				std::cerr << "WritingError: Log file is not open for writing." << std::endl;
			}
			break;
		}
	}
	template<typename... Args>
	void debug(const Args&... args)
	{
		std::ostringstream oss = format_with_time(LogLevel::Debug, args...);

		switch (target)
		{
		case LogTarget::None:
			break;
		case LogTarget::Console:
			setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE);
			std::cout << oss.str() << std::endl;
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case LogTarget::File:
			if (log_file.is_open())
				log_file << oss.str() << std::endl;
			else
			{
				setConsoleColor(FOREGROUND_RED);
				std::cerr << "WritingError: Log file is not open for writing." << std::endl;
			}
			break;
		}
	}
	template<typename... Args>
	void trace(const Args&... args)
	{
		std::ostringstream oss = format_with_time(LogLevel::Trace, args...);

		switch (target)
		{
		case LogTarget::None:
			break;
		case LogTarget::Console:
			setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE);
			std::cout << oss.str() << std::endl;
			setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case LogTarget::File:
			if (log_file.is_open())
				log_file << oss.str() << std::endl;
			else
			{
				setConsoleColor(FOREGROUND_RED);
				std::cerr << "WritingError: Log file is not open for writing." << std::endl;
			}
			break;
		}
	}

	// 根据level打印到target
	template<typename... Args>
	void printlog(const Args&... args)
	{
		switch (level)
		{
		case LogLevel::Info:
			info(args...);
			break;
		case LogLevel::Warn:
			warn(args...);
			break;
		case LogLevel::Error:
			error(args...);
			break;
		case LogLevel::Debug:
			debug(args...);
			break;
		case LogLevel::Trace:
			trace(args...);
			break;
		}
	}

private:
	Log(const std::string& filename = "file.log");
	~Log();
	
	LogLevel level;
	LogTarget target;

	std::ofstream log_file;  // 日志文件流对象
};




#endif // !_LOG_H_