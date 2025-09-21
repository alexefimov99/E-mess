#ifndef LOGGER_H
#define LOGGER_H

#include "own_utils.h"

#include <string>
#include <string_view>
#include <filesystem>


class Logger
{
public:
    enum class Level {
        INFO,
        WARNING,
        ERROR
    };

public:
    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;

    template<typename... Args>
    void info(Args&&... args) {
        if (m_definite_level > Level::INFO) {
            return;
        }

        std::stringstream log_message;
        log_message << TimeUtils::nowStr() << " [INFO] ";
        (log_message << ... << std::forward<Args>(args));
        writeMessage(log_message);
    }

    template<typename... Args>
    void warning(Args&&... args) {
        if (m_definite_level > Level::WARNING) {
            return;
        }

        std::stringstream log_message;
        log_message << TimeUtils::nowStr() << " [WARNING] ";
        (log_message << ... << std::forward<Args>(args));
        writeMessage(log_message);
    }

    template<typename... Args>
    void error(Args&&... args) {
        if (m_definite_level > Level::ERROR) {
            return;
        }

        std::stringstream log_message;
        log_message << TimeUtils::nowStr() << " [ERROR] ";
        (log_message << ... << std::forward<Args>(args));
        writeMessage(log_message);
    }

    static Logger* const getInstance(const Level level = Level::INFO);

private:
    static Logger* m_logger_instance;
    Level m_definite_level;

    std::string_view log_dir;

private:
    Logger(const Level level);

    bool fileExist();
    std::filesystem::path getCurrPath();
    std::filesystem::path getLogPath();
    std::string getFilename(const std::string& new_name = "");
    // TODO: update this method for multiply thread
    std::string generateFileName();
    std::pair<std::filesystem::path, std::uintmax_t> GetLatestFileInfo();

    void writeMessage(const std::stringstream& log_message);
    void writeInFile(const std::stringstream& log_message);
};

#endif // LOGGER_H
