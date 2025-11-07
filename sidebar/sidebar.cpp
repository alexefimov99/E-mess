#include "sidebar.h"

#include <QVBoxLayout>
#include <QMouseEvent>

Sidebar::Sidebar(QWidget* parent, const SidebarType sidebar_type) : QWidget(parent)
    , m_sidebar_type(sidebar_type) {
    setFixedSize(m_SIZE, m_SIZE);

    initSidebar();

    hide();
}

void Sidebar::initSidebar() {
    m_self_dialog = new QPushButton(this);
    m_self_dialog->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_self_dialog->setFlat(true);

    m_self_dialog->setIcon(QIcon("/home/aleksandr/Dev/0_Projects/Qt/E-mess/icons/self_chat.png"));
    m_self_dialog->setIconSize(QSize(m_SIZE, m_SIZE));
    m_self_dialog->setFixedSize(m_SIZE, m_SIZE);
}

void Sidebar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        switch (m_sidebar_type) {
        case SidebarType::CONTACTS:
            m_log->info("Sidebar type is CONTACTS");
            break;
        case SidebarType::CONTACTS_SETTINGS:
            m_log->info("Sidebar type is CONTACTS_SETTINGS");
            break;
        case SidebarType::USER_SETTINGS:
            m_log->info("Sidebar type is USER_SETTINGS");
            break;
        default:
            break;
        }
    }

    QWidget::mousePressEvent(event);
}

void Sidebar::detailedPositionLog(QMouseEvent* event) const {
    m_log->info("event pos = ",  event->pos().x(), ", ", event->pos().y());
    m_log->info("global pos = ", event->globalPos().x(), ", ", event->globalPos().y());

    m_log->info("geometry = ", geometry().x(), ", ", geometry().y(), ", ",
                               geometry().x() + geometry().width(), ", ",
                               geometry().y() + geometry().height());

    m_log->info("rect = ", rect().x(), ", ",
                           rect().y(), ", ",
                           rect().x() + rect().width(), ", ",
                           rect().y() + rect().height());
}

Sidebar::~Sidebar() {

}
