#ifndef WIDGET_H
#define WIDGET_H

#include "chats/chats_common.h"
#include "sidebar/sidebar.h"
#include "utils/logger.h"

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QPropertyAnimation>


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

private:
    Ui::Widget* ui;
    std::shared_ptr<Logger> m_log;

    QGridLayout* m_main_layout;

    QPropertyAnimation* m_sidebar_animation;
    Sidebar* m_sidebar;
    QTimer* m_auto_hide_timer;

    ChatsCommon* m_common_elements;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void createSidebar();
    void createGeneralWidget();
    void updateConnectionStatus();

    void keyPressEvent(QKeyEvent* key_event) override;

private slots:
    void showSidebar();
    void hideSidebar();

    // void onMessageReceived(const QByteArray &data);
};
#endif  // WIDGET_H
