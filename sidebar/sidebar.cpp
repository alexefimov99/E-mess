#include "sidebar.h"

Sidebar::Sidebar(QWidget* parent) : QWidget(parent) {
    setFixedWidth(200);
    setStyleSheet("background-color: green; border-radius: 30px; ");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    m_self_dialog = new QPushButton(this);

    QIcon icon("/home/aleksandr/Dev/0_Projects/Qt/E-mess/icons/self_chat.png");
    m_self_dialog->setIcon(icon);
    m_self_dialog->setFixedSize(m_SIZE, m_SIZE);
    layout->addWidget(m_self_dialog);

    hide();
}

Sidebar::~Sidebar() {

}
