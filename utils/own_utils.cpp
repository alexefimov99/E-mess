#include "own_utils.h"

#include <ctime>
#include <QApplication>

#include "../sidebar/sidebar.h"
#include "../utils/logger.h"


static Logger* log = Logger::getInstance();

QList<Sidebar*> findAllSidebars(const QWidget* widget) {
    QList<Sidebar*> sidebars = widget->findChildren<Sidebar*>();
    return sidebars;
}

namespace Utils {
    void closeWindow() {
        // TODO: Close the application window
        exitFromApp();
    }

    void exitFromApp() {
        log->info("Exit from app");
        // TODO: Quit from the application
        exit(0);
    }

    void showSidebars(const QWidget* main_widget) {
        static QList<Sidebar*> sidebars;
        if (sidebars.empty()) {
            sidebars = findAllSidebars(main_widget);
        }

        for (Sidebar* sidebar : sidebars) {
            sidebar->show();
        }
    }

    void hideSidebars(const QWidget* main_widget) {
        static QList<Sidebar*> sidebars;
        if (sidebars.empty()) {
            sidebars = findAllSidebars(main_widget);
        }

        for (Sidebar* sidebar : sidebars) {
            sidebar->hide();
        }
    }

    std::filesystem::path getPath(const std::string_view& dir) {
        std::filesystem::path curr_path;
        try {
            curr_path = std::filesystem::current_path() / dir;
        } catch(const std::filesystem::filesystem_error& err) {
            log->warning("Can't take path to application. Check rights");
            curr_path = std::filesystem::path("/home") / dir; // TODO: Update also for win
        }

        return curr_path;
    }
} // namespace Utils

namespace TimeUtils {
    std::string_view timestampToString(const auto& curr_timestamp, bool full_date) {
        std::time_t time = std::chrono::system_clock::to_time_t(curr_timestamp);

        std::tm tm;
        localtime_r(&time, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, full_date ? "%Y-%m%d %H:%M:%S" : "%H:%M:%S");

        return oss.view();
    }

    std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }

    std::string_view nowStr() {
        auto curr_time = now();
        return timestampToString(curr_time);
    }

} // namespace TimeUtils
