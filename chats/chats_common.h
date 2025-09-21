#ifndef CHATS_COMMON_H
#define CHATS_COMMON_H

#include "user_message_box.h"

#include <QTextEdit>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QSpacerItem>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QList>

#include <iostream>

class ChatsCommon : public QWidget
{
    Q_OBJECT
public:
    ChatsCommon(QWidget* parent = nullptr);
    ~ChatsCommon();

    // constexpr int getTextEditSize() const {
    //     return TEXT_EDIT_SIZE;
    // }

private:
    void paintEvent(QPaintEvent* event);

    void installDialogIcons();

    void createScrollArea();
    void createMessageLabel(const QString& message, const Qt::AlignmentFlag align_side);
    // QString wrappedText(const QString& message, const int max_width, const QFontMetrics& metrics);
    // TODO: Delete or write
    QSize calcMessageLabelSize();

private:
    static constexpr int m_TEXT_EDIT_SIZE = 75;

    QVBoxLayout* m_layout;

    QScrollArea* m_scroll_area;
    QWidget*     m_scroll_widget;
    QVBoxLayout* m_scroll_layout;

    QHBoxLayout* m_message_layout;
    // QSpacerItem* message_layout_spacer;
    const QSize m_MESSAGE_INDENT { 100, 10 };
    // Try to calculate m_DEFAULT_INDENT
    const int m_DEFAULT_INDENT { 30 };
    int m_old_parent_width;

    QHBoxLayout* m_layout_message_box;
    UserMessageBox* m_message_box;

public slots:
    void messageSending(const QString& message);
    void messageReceiving(const QString& message);
};

#endif  // CHATS_COMMON_H
