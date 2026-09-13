#include "log.h"

Log* Log::getInstance()
{
    static Log log;
    log.setTarget(LogTarget::Console);
    log.info("log instance created.");
    return &log;
}

Log::Log(const std::string& filename)
{
    target = LogTarget::Console;

    log_file.open(filename, std::ios::app);  // 追加模式打开文件
    if (!log_file.is_open())
    {
        setConsoleColor(FOREGROUND_RED);
        std::cerr << "InitError: Failed to open log file: " << filename << std::endl;
    }

    level = LogLevel::Info;
}

Log::~Log()
{
    if (log_file.is_open()) {
        log_file.close();
    }
}

