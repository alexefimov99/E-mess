#ifndef USER_MESSAGE_BOX_H
#define USER_MESSAGE_BOX_H

#include "../utils/own_utils.h"

#include <QTextEdit>
#include <QKeyEvent>

#include <iostream>

class UserMessageBox : public QTextEdit
{
    Q_OBJECT
public:
    UserMessageBox(QWidget* parent = nullptr);
    ~UserMessageBox();

private:
    void keyPressEvent(QKeyEvent* key_event) override;

    void lineBreaker(size_t pos, QString& text);

signals:
    void sendMessage(const QString& message);
};

#endif  // USER_MESSAGE_BOX_H
