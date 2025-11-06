#ifndef OWN_UTILS_H
#define OWN_UTILS_H

#include <chrono>
#include <string_view>

#include <QWidget>


namespace Utils {
    void closeWindow();
    void exitFromApp();

    void showSidebars(QWidget* main_widget);
    void hideSidebars(QWidget* main_widget);
} // namespace Utils

namespace TimeUtils {
    std::string_view timestampToString(const auto& curr_timestamp, bool full_date = false);

    std::chrono::system_clock::time_point now();
    std::string_view nowStr();
} // namespace TimeUtils

#endif // OWN_UTILS_H
