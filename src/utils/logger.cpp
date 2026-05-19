#include "logger.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <limits.h>


Logger::Logger(const Level level)
    : m_definite_level(level)
    , m_log_dir("Logs") { }

std::shared_ptr<Logger> Logger::getInstance(const Level level) {
    static std::shared_ptr<Logger> instance{new Logger(level)};
    return instance;
}

std::filesystem::path Logger::getLogPath() {
    return Utils::getPath(m_log_dir);
}

std::string Logger::getFilename(const std::string& new_name) {
    static std::string log_filename;
    if (!new_name.empty()) {
        log_filename = new_name;
    }

    return log_filename;
}

bool Logger::fileExist() {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(getLogPath())) {
            // std::cout << entry.path().string() << std::endl;

            if (!entry.is_directory() && entry.path().filename().string() == getFilename()) {
                return true;
            }
        }

        return false;
    } catch (const std::filesystem::filesystem_error& err) {
        std::cout << "Filesystem access error: " <<  err.what() << std::endl;
    }

    return false;
}

std::string Logger::generateFileName() {
    std::stringstream filename;
    filename << "log_" << TimeUtils::nowStr() << ".txt";

    return filename.str();
}

std::pair<std::filesystem::path, std::uintmax_t> Logger::GetLatestFileInfo() {
    std::filesystem::path latest_file;
    std::uintmax_t file_size = 0;
    std::time_t latest_time_access = LONG_MIN;

    for (const auto& entry : std::filesystem::directory_iterator(getLogPath())) {
        if (entry.is_regular_file()) {
            // TODO: Why is it negative time?
            const auto write_time = entry.last_write_time();
            const auto time_since_epoch = write_time.time_since_epoch();
            const auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

            if (seconds_since_epoch > latest_time_access) {
                latest_time_access = seconds_since_epoch;
                latest_file = entry.path();
                file_size = entry.file_size();
            }
        }
    }

    return {latest_file, file_size};
}

void Logger::writeInFile(const std::stringstream& log_message) {
    static constexpr std::uint32_t FIVE_MBYTES = 5 /** 1024*/ * 1024;
    const auto [latest_file, file_size] = GetLatestFileInfo();
    const bool file_size_exceeded = file_size > FIVE_MBYTES;
    if (getFilename().empty() || file_size_exceeded) {
        const std::string actual_name = latest_file.empty() || file_size_exceeded ? generateFileName() : latest_file.filename().string();
        // TODO: maybe I should make a set function
        getFilename(actual_name);
    }

    std::fstream file(getLogPath() / getFilename(), std::ios::app);
    if (!file) {
        throw std::runtime_error("Cannot open the log file" + (getLogPath() / getFilename()).string());
    }

    file << log_message.str() << '\n';
}

void Logger::writeMessage(const std::stringstream& log_message) {
    std::cout << log_message.str() << std::endl;

    if (!std::filesystem::directory_entry(getLogPath()).is_directory()) {
        std::filesystem::create_directory(getLogPath());
    }
    writeInFile(log_message);
}
