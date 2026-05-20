#include "user_message_box.h"

#include "../../utils/own_utils.h"

#include "../sidebar/sidebar.h"


UserMessageBox::UserMessageBox(QWidget* parent)
    : QTextEdit(parent)
    , m_root_widget(parent->parentWidget()) {
    setStyleSheet("border-radius: 10px; font-size: 12pt; background: #2c2c2c; color: cyan");
}

UserMessageBox::~UserMessageBox() {

}

void UserMessageBox::keyPressEvent(QKeyEvent* key_event) {
    int symbol_code = key_event->key();
    const char* symb = key_event->text().toUtf8().constData();

    QTextCursor text_cursor { textCursor() };

    if (symbol_code == Qt::Key_Backspace) {
        if (!text_cursor.atStart()) {
            text_cursor.deletePreviousChar();
        }
        return;
    } else if (symbol_code == Qt::Key_Delete) {
        if (!text_cursor.atEnd()) {
            text_cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            text_cursor.removeSelectedText();
            setTextCursor(text_cursor);
        }
        return;
    }

    if (key_event->modifiers() == Qt::ShiftModifier && (symbol_code == Qt::Key_Return
                                                    || symbol_code == Qt::Key_Enter)) {
        text_cursor.insertText("\n");
        setTextCursor(text_cursor);
        return;
    }

    if (key_event->modifiers() == Qt::ControlModifier) {
        if (symbol_code == Qt::Key_Q) {
            Utils::exitFromApp();
        } else if (symbol_code == Qt::Key_W) {
            Utils::closeWindow();
        } else if (symbol_code == Qt::Key_Return ||
                   symbol_code == Qt::Key_Enter) {
            if (!toPlainText().isEmpty()) {
                text_cursor.insertText("\n");
                setTextCursor(text_cursor);
            }
            return;
        } else if (symbol_code == Qt::Key_C) {
            copy();
            return;
        } else if (symbol_code == Qt::Key_V) {
            paste();
            return;
        }
    }

    if (symbol_code == Qt::Key_Return && !toPlainText().isEmpty()) {
        emit(sendMessage(toPlainText()));
        text_cursor.select(QTextCursor::SelectionType::Document);
        text_cursor.removeSelectedText();
        return;
    }

    switch (symbol_code) {
    case Qt::Key_Left:
        text_cursor.movePosition(QTextCursor::Left);
        break;
    case Qt::Key_Up:
        text_cursor.movePosition(QTextCursor::Up);
        break;
    case Qt::Key_Right:
        text_cursor.movePosition(QTextCursor::Right);
        break;
    case Qt::Key_Down:
        text_cursor.movePosition(QTextCursor::Down);
        break;
    case Qt::Key_Control:
        break;
    default:
        text_cursor.insertText(symb);
        break;
    }

    setTextCursor(text_cursor);
}

void UserMessageBox::keyReleaseEvent(QKeyEvent* key_event) {
    const int symbol_code = key_event->key();
    if (symbol_code == Qt::Key_Alt) {
        const bool sidebars_visible = m_root_widget->findChildren<Sidebar*>().takeFirst()->isVisible();
        if (sidebars_visible) {
            Utils::hideSidebars(m_root_widget);
        } else {
            Utils::showSidebars(m_root_widget);
        }

        return;
    }
}
