#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QWidget>

#include <memory>

class Logger;


class NotificationManager : public QWidget {
public:
    NotificationManager(QWidget* parent = nullptr);
    ~NotificationManager();

protected:
    void showEvent(QShowEvent* ev) override;

private:
    std::shared_ptr<Logger> m_log;
};

#endif // NOTIFICATION_MANAGER_H
