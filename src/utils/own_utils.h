#ifndef OWN_UTILS_H
#define OWN_UTILS_H

#include <chrono>
#include <filesystem>
#include <string_view>

#include <QWidget>


namespace Utils {
    void closeWindow();
    void exitFromApp();

    void showSidebars(const QWidget* main_widget);
    void hideSidebars(const QWidget* main_widget);

    std::filesystem::path getPath(const std::string_view& dir);
} // namespace Utils

namespace TimeUtils {
    std::string timestampToString(const auto& curr_timestamp, bool full_date = false);

    std::chrono::system_clock::time_point now();
    std::string nowStr();
} // namespace TimeUtils

#endif // OWN_UTILS_H
