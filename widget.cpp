#include "widget.h"
#include "./ui_widget.h"

#include "utils/own_utils.h"


static std::atomic_bool s_should_kill_network_thread = false;

Widget::Widget(QWidget *parent)
               : QWidget(parent)
               , ui(new Ui::Widget) {
    ui->setupUi(this);
    setWindowTitle("E-mess");

    m_main_layout = new QGridLayout(this);

    createGeneralWidget();
    createSidebar();

    m_main_layout->addWidget(m_common_elements);
    setLayout(m_main_layout);
}

Widget::~Widget() {
    delete ui;
}

void Widget::onMessageReceived(const QByteArray &data) {
    std::cout << "Received data: " << QString(data).toUtf8().constData() << std::endl;
}

bool Widget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseMove) {
        auto mouse_event = static_cast<QMouseEvent*>(event);
        int x = mouse_event->pos().x();

        if (x < 25) {
            showSidebar();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void Widget::createGeneralWidget() {
    m_common_elements = new ChatsCommon(this);
}

void Widget::createSidebar() {
    m_sidebar = new Sidebar(this);
    m_sidebar->move(0, 0);

    m_auto_hide_timer = new QTimer(this);
    m_auto_hide_timer->setSingleShot(true);
    connect(m_auto_hide_timer, &QTimer::timeout, this, &Widget::hideSidebar);

    setMouseTracking(true);
    installEventFilter(this);
}

void Widget::showSidebar() {
    if (!m_sidebar->isVisible()) {
        m_sidebar->show();
    }

    m_sidebar_animation = new QPropertyAnimation(m_sidebar, "geometry");
    m_sidebar_animation->setDuration(300);
    m_sidebar_animation->setStartValue(QRect(-m_sidebar->width(), 0, m_sidebar->width(), height()));
    m_sidebar_animation->setEndValue(QRect(0, 0, m_sidebar->width(), height()));
    m_sidebar_animation->start(QAbstractAnimation::DeleteWhenStopped);

    m_auto_hide_timer->start(3000);
}

void Widget::hideSidebar() {
    if (!m_sidebar->underMouse()) {
        m_sidebar_animation = new QPropertyAnimation(m_sidebar, "geometry");
        m_sidebar_animation->setDuration(300);
        m_sidebar_animation->setStartValue(QRect(0, 0, m_sidebar->width(), height()));
        m_sidebar_animation->setEndValue(QRect(-m_sidebar->width(), 0, m_sidebar->width(), height()));
        m_sidebar_animation->start(QAbstractAnimation::DeleteWhenStopped);

        connect(m_sidebar_animation, &QPropertyAnimation::finished, m_sidebar, &Sidebar::hide);
    }
}

void Widget::keyPressEvent(QKeyEvent* key_event) {
    int symbol_code = key_event->key();
    const char* symb = key_event->text().toUtf8().constData();

    if (key_event->modifiers() == Qt::ControlModifier) {
        if (symbol_code == Qt::Key_Q) {
            Utils::exitFromApp();
        } else if (symbol_code == Qt::Key_W) {
            Utils::closeWindow();
        }
    }
}

void Widget::paintEvent(QPaintEvent* event) {
    m_main_layout->update();
}
