#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>

#include "../utils/logger.h"


class Sidebar : public QWidget {
    Q_OBJECT
public:
    enum class SidebarType {
        CONTACTS,
        CONTACTS_SETTINGS,
        USER_SETTINGS
    };

public:
    Sidebar(QWidget* parent, SidebarType sidebar_type);
    ~Sidebar();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    Logger* m_log = Logger::getInstance();

    QPushButton* m_self_dialog;
    const int m_SIZE = 60;
    const SidebarType m_sidebar_type;

private:
    void initSidebar();
    void detailedPositionLog(QMouseEvent* event) const;
};

#endif  // SIDEBAR_H
