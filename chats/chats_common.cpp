#include "chats_common.h"

#include <QLabel>
#include <QList>
#include <QScrollBar>
#include <QSpacerItem>

#include <iostream>


ChatsCommon::ChatsCommon(QWidget* parent) : QWidget(parent), m_old_parent_width(parentWidget()->width()) {
    m_layout = new QVBoxLayout(this);

    // message_layout_spacer = new QSpacerItem(MESSAGE_INDENT.width(), MESSAGE_INDENT.height(), QSizePolicy::Minimum, QSizePolicy::Minimum);
    createScrollArea();
    m_layout->addWidget(m_scroll_area);

    m_message_box = new UserMessageBox(this);
    m_message_box->setMaximumHeight(m_TEXT_EDIT_SIZE);
    m_message_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_layout_message_box = new QHBoxLayout(this);
    m_layout_message_box->addWidget(m_message_box);

    m_layout->addLayout(m_layout_message_box);
    setLayout(m_layout);

    connect(m_message_box, &UserMessageBox::sendMessage, this, &ChatsCommon::messageSending);
    connect(m_message_box, &UserMessageBox::sendMessage, this, &ChatsCommon::messageReceiving);
}

ChatsCommon::~ChatsCommon() {

}

void ChatsCommon::createScrollArea() {
    m_scroll_area   = new QScrollArea(this);
    m_scroll_widget = new QWidget(this);
    m_scroll_layout = new QVBoxLayout(m_scroll_widget);
    m_scroll_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_scroll_widget->setLayout(m_scroll_layout);
    m_scroll_area->setWidget(m_scroll_widget);

    m_scroll_area->setWidgetResizable(true);
    m_scroll_area->setFrameShape(QFrame::NoFrame);
    m_scroll_area->horizontalScrollBar()->setEnabled(false);
    m_scroll_area->horizontalScrollBar()->setStyleSheet("QScrollBar { height: 0px; }");
    m_scroll_widget->setFixedWidth(parentWidget()->width() - m_DEFAULT_INDENT);
}

// QString ChatsCommon::wrappedText(const QString& message, const int max_width, const QFontMetrics& metrics) {
//     QString result;
//     QString line;

//     for (const QChar c : message) {
//         line += c;
//         if (metrics.horizontalAdvance(line) > max_width) {
//             result += line + '\n';
//             line.clear();
//         }
//     }

//     result += line;
//     return result;
// }

void ChatsCommon::createMessageLabel(const QString& message, const Qt::AlignmentFlag align_side) {
    m_message_layout = new QHBoxLayout(m_scroll_widget);

    QLabel* message_label = new QLabel(message, m_scroll_widget);
    message_label->setFixedWidth(parentWidget()->width() - m_MESSAGE_INDENT.width() - m_DEFAULT_INDENT);
    // TODO: Make a file settings for font-sizes, paddings etc.
    message_label->setStyleSheet(
        "QLabel { "
        "border-radius: 15px; "
        "background: #3c3c3c; "
        "color: cyan; "
        "font-size: 12pt; "
        "word-break: break-all; "
        "padding: 5px; "
        "margin-right: 15px; "
        "} "
    );
    message_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    message_label->setWordWrap(true);

    // QFontMetrics metrics(message_label->font());
    // message_label->setText(wrappedText(message, max_width, metrics));

    // message_layout->addSpacerItem(message_layout_spacer);
    m_message_layout->addWidget(message_label, 0, align_side);
    m_scroll_layout->addLayout(m_message_layout);
    m_scroll_layout->setAlignment(m_message_layout, Qt::AlignBottom);

    m_scroll_widget->adjustSize();
    m_scroll_area->verticalScrollBar()->setEnabled(true);
    m_scroll_area->verticalScrollBar()->setValue(m_scroll_area->verticalScrollBar()->maximum());
}

void ChatsCommon::messageSending(const QString& message) {
    createMessageLabel(message, Qt::AlignRight);
    // Serialization message
    std::cout << message.toUtf8().constData() << std::endl;
}

void ChatsCommon::messageReceiving(const QString& message) {
    createMessageLabel(message, Qt::AlignLeft);
    // Deserialization message
    std::cout << message.toUtf8().constData() << std::endl;
}

void ChatsCommon::paintEvent(QPaintEvent* event) {
    if (m_old_parent_width != parentWidget()->width()) {
        m_scroll_widget->setFixedWidth(parentWidget()->width() - m_DEFAULT_INDENT);

        QList<QLabel*> label_list = m_scroll_widget->findChildren<QLabel*>();
        foreach (QLabel* l, label_list) {
            l->setFixedWidth(parentWidget()->width() - m_MESSAGE_INDENT.width() - m_DEFAULT_INDENT);
        }
        m_old_parent_width = parentWidget()->width();
    }
}
