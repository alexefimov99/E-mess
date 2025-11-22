#include "widget.h"
#include "./ui_widget.h"

#include "notifications/notification_manager.h"
#include "settings/settings.h"
#include "utils/own_utils.h"

#include <QMouseEvent>


static std::atomic_bool s_should_kill_network_thread = false;

Widget::Widget(QWidget *parent)
               : QWidget(parent)
               , ui(new Ui::Widget)
               , m_log(Logger::getInstance()) {
    ui->setupUi(this);
    m_log->info("Launch app");
    setWindowTitle("E-mess");

    m_main_layout = new QGridLayout(this);

    createGeneralWidget();
    initSidebars();
    loadData();

    m_main_layout->addWidget(m_common_elements);
    setLayout(m_main_layout);

    notifier = new NotificationManager(this);
}

Widget::~Widget() {
    delete ui;
}

// void Widget::onMessageReceived(const QByteArray &data) {
//     m_log->info("Received data: ", QString(data).toUtf8().constData());
// }

// bool Widget::eventFilter(QObject* watched, QEvent* event) {
//     if (event->type() == QEvent::MouseMove) {
//         auto mouse_event = static_cast<QMouseEvent*>(event);
//         int x = mouse_event->pos().x();

//         if (x < 25) {
//             // showSidebar();
//             // Utils::showSidebars(m_companion_sidebar, m_sidebar_animation);
//         }
//     }

//     return QWidget::eventFilter(watched, event);
// }

void Widget::createGeneralWidget() {
    m_common_elements = new ChatsCommon(this);
}

void Widget::initSidebars() {
    // m_auto_hide_timer = new QTimer(this);
    // m_auto_hide_timer->setSingleShot(true);
    // connect(m_auto_hide_timer, &QTimer::timeout, this, &Widget::hideSidebar);

    // setMouseTracking(true);
    // installEventFilter(this);
    QHBoxLayout* sidebars_layout = new QHBoxLayout(this);

    m_contact_list_sidebar = new Sidebar(this, Sidebar::SidebarType::CONTACTS);
    m_contact_settings_sidebar = new Sidebar(this, Sidebar::SidebarType::CONTACTS_SETTINGS);
    m_user_settings_sidebar = new Sidebar(this, Sidebar::SidebarType::USER_SETTINGS);
    sidebars_layout->addWidget(m_contact_list_sidebar, 0, Qt::AlignLeft);
    sidebars_layout->addWidget(m_contact_settings_sidebar, 0, Qt::AlignLeft);
    sidebars_layout->addStretch(0);
    sidebars_layout->addWidget(m_user_settings_sidebar, 0, Qt::AlignRight);
    m_main_layout->addLayout(sidebars_layout, 0, 0);
}

void Widget::loadData() {
    // loadSettings();
    contact_settings = new ContactSettings();
    user_settings = new UserSettings();
    user_settings->settingsInit();

    // auto us = dynamic_cast<UserSettings*>(user_settings);
    // QSize window_size = us->getWindowSize();

    // loadChatsHistory();
}

void Widget::keyPressEvent(QKeyEvent* key_event) {
    const int symbol_code = key_event->key();
    if (key_event->modifiers() == Qt::ControlModifier) {
        if (symbol_code == Qt::Key_Q) {
            Utils::exitFromApp();
        } else if (symbol_code == Qt::Key_W) {
            Utils::closeWindow();
        }
    }
}

void Widget::keyReleaseEvent(QKeyEvent* key_event) {
    const int symbol_code = key_event->key();
    if (symbol_code == Qt::Key_Alt) {
        if (m_contact_list_sidebar->isVisible()) {
            Utils::hideSidebars(this);
        } else {
            Utils::showSidebars(this);
        }
    }
}

void Widget::paintEvent(QPaintEvent* event) {
    m_main_layout->update();
}
