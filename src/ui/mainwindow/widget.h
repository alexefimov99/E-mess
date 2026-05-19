#ifndef WIDGET_H
#define WIDGET_H

#include "../chats/chats_common.h"
#include "../sidebar/sidebar.h"
#include "../../utils/logger.h"

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QPropertyAnimation>

class Settings;
class NotificationManager;


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::Widget* ui;
    std::shared_ptr<Logger> m_log;

    QGridLayout* m_main_layout;

    // QPropertyAnimation* m_sidebar_animation;
    Sidebar* m_contact_list_sidebar;
    Sidebar* m_contact_settings_sidebar;
    Sidebar* m_user_settings_sidebar;
    // QTimer* m_auto_hide_timer;

    ChatsCommon* m_common_elements;

    Settings* contact_settings;
    Settings* user_settings;

    NotificationManager* notifier;

private:
    void createGeneralWidget();
    void initSidebars();
    void loadData();
    // void updateConnectionStatus();

    void keyPressEvent(QKeyEvent* key_event) override;
    void keyReleaseEvent(QKeyEvent* key_event) override;

private slots:
    // void onMessageReceived(const QByteArray &data);
};
#endif  // WIDGET_H
