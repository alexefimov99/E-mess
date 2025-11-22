#include "notification_manager.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QRect>

#include "../utils/logger.h"


NotificationManager::NotificationManager(QWidget* parent) : QWidget(parent), m_log(Logger::getInstance()) {
    setWindowFlag(Qt::ToolTip);
    // setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout* notifier_layout = new QHBoxLayout(this);
    QLabel* label = new QLabel("sometext", this);
    label->setStyleSheet("QLabel { color: white; }");
    notifier_layout->addWidget(label);
    setStyleSheet("background: rgba(50,50,50,255); border-radius: 8px; padding: 8px;");

    show();
}

NotificationManager::~NotificationManager() {

}

void NotificationManager::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    QWidget* parent = parentWidget();
    if (!parent) {
        return;
    }

    QPoint parentPos = parent->mapToGlobal(QPoint(0, 0));
    QSize parentSize = parent->size();
    QSize popupSize = sizeHint();

    QPoint popupPos(
        parentPos.x() + parentSize.width() - popupSize.width() - 10,
        parentPos.y() + parentSize.height() - popupSize.height() - 10
    );

    move(popupPos);
}
