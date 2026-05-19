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

    static std::shared_ptr<Logger> getInstance(const Level level = Level::INFO);

private:
    Level m_definite_level;

    std::string_view m_log_dir;

private:
    explicit Logger(const Level level);

    [[nodiscard]] bool fileExist();
    [[nodiscard]] std::filesystem::path getCurrPath();
    [[nodiscard]] std::filesystem::path getLogPath();
    std::string getFilename(const std::string& new_name = "");
    // TODO: update this method for multiply thread
    [[nodiscard]] std::string generateFileName();
    [[nodiscard]] std::pair<std::filesystem::path, std::uintmax_t> GetLatestFileInfo();

    void writeMessage(const std::stringstream& log_message);
    void writeInFile(const std::stringstream& log_message);
};

#endif // LOGGER_H
