#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>


class Sidebar : public QWidget {
    Q_OBJECT
public:
    Sidebar(QWidget* parent = nullptr);
    ~Sidebar();

private:
    QPushButton* m_self_dialog;
    const int m_SIZE = 60;
};

#endif  // SIDEBAR_H
