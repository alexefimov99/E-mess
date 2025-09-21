#include "networkconnecion.h"

#include "../utils/logger.h"


NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_work(std::make_shared<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(m_io_context)))
    , m_socket(std::make_shared<tcp::socket>(m_io_context))
    , m_state(Disconnected)
    , m_port(0)
    , m_stopping(false)
    , m_isWriting(false)
{
    m_receiveBuffer.resize(BUFFER_SIZE);
    m_thread = std::thread([this]() { runAsioThread(); });

    connect(&m_eventProcessTimer, &QTimer::timeout, this, &NetworkClient::processAsioEvents);
    m_eventProcessTimer.start(EVENT_PROCESS_INTERVAL);

    connect(&m_reconnectTimer, &QTimer::timeout, this, &NetworkClient::attemptReconnect);
}

NetworkClient::~NetworkClient() {
    disconnect();
    stopAsioThread();
}

void NetworkClient::connectToServer(const QString &host, int port) {
    if (m_state == Connecting || m_state == Connected) {
        return;
    }

    m_host = host;
    m_port = port;

    setConnectionState(Connecting);
    Log::info("[NETWORK] Connecting to ", host.toUtf8().constData(), ":", port);

    asio::post(m_io_context, [this]() {
        try {
            tcp::resolver resolver(m_io_context);
            auto endpoints = resolver.resolve(m_host.toStdString(), std::to_string(m_port));

            asio::async_connect(*m_socket, endpoints,
                                [this](const boost::system::error_code& error, const tcp::endpoint&) {
                                    if (!error) {
                                        boost::system::error_code ec;
                                        m_socket->set_option(asio::socket_base::keep_alive(true), ec);

                                        setConnectionState(Connected);
                                        startReadOperation();
                                    } else {
                                        m_errorString = QString::fromStdString(error.message());
                                        setConnectionState(Error);
                                        m_reconnectTimer.start(RECONNECT_INTERVAL);
                                    }
                                });
        } catch (const std::exception &e) {
            m_errorString = QString("Connection error: %1").arg(e.what());
            setConnectionState(Error);
            m_reconnectTimer.start(RECONNECT_INTERVAL);
        }
    });
}

void NetworkClient::disconnect() {
    m_reconnectTimer.stop();

    if (m_state == Disconnected) {
        return;
    }

    asio::post(m_io_context, [this]() {
        boost::system::error_code ec;
        if (m_socket->is_open()) {
            m_socket->shutdown(asio::socket_base::shutdown_both, ec);
            m_socket->close(ec);
        }

        m_socket = std::make_shared<tcp::socket>(m_io_context);
        setConnectionState(Disconnected);
    });
}

void NetworkClient::sendMessage(const QByteArray &data) {
    if (m_state != Connected) {
        return;
    }

    m_sendQueue.enqueue(data);

    if (!m_isWriting) {
        asio::post(m_io_context, [this]() {
            if (!m_isWriting && !m_sendQueue.isEmpty()) {
                m_isWriting = true;
                QByteArray data = m_sendQueue.dequeue();

                asio::async_write(*m_socket,
                                  asio::buffer(data.constData(), data.size()),
                                  [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                      handleWrite(error, bytes_transferred);
                                  }
                                  );
            }
        });
    }
}

NetworkClient::ConnectionState NetworkClient::state() const {
    return m_state;
}

QString NetworkClient::errorString() const {
    return m_errorString;
}

void NetworkClient::processAsioEvents() {

}

void NetworkClient::attemptReconnect() {
    if (m_state == Disconnected || m_state == Error) {
        Log::warning("[NETWORK] Attempting to reconnect...");
        connectToServer(m_host, m_port);
    }
}

void NetworkClient::startReadOperation() {
    if (!m_socket->is_open()) {
        return;
    }

    m_socket->async_read_some(
        asio::buffer(m_receiveBuffer),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handleRead(error, bytes_transferred);
        }
        );
}

void NetworkClient::handleRead(const boost::system::error_code &error, std::size_t bytesTransferred) {
    if (!error) {
        QByteArray data = QByteArray::fromRawData(m_receiveBuffer.data(), bytesTransferred);
        emit messageReceived(data);
        startReadOperation();
    } else {
        if (error != asio::error::operation_aborted) {
            m_errorString = QString::fromStdString(error.message());
            emit errorOccurred(m_errorString);

            setConnectionState(Disconnected);
            m_reconnectTimer.start(RECONNECT_INTERVAL);
        }
    }
}

void NetworkClient::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred) {
    if (error) {
        m_errorString = QString::fromStdString(error.message());
        emit errorOccurred(m_errorString);

        setConnectionState(Disconnected);
        m_reconnectTimer.start(RECONNECT_INTERVAL);
        return;
    }

    if (!m_sendQueue.isEmpty()) {
        QByteArray data = m_sendQueue.dequeue();

        asio::async_write(*m_socket,
                          asio::buffer(data.constData(), data.size()),
                          [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                              handleWrite(error, bytes_transferred);
                          });
    } else {
        m_isWriting = false;
    }
}

void NetworkClient::setConnectionState(ConnectionState newState) {
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}

void NetworkClient::runAsioThread() {
    try {
        m_io_context.run();
    } catch (const std::exception &e) {
        Log::error("[NETWORK] Exception in network thread: ", e.what());
    }
}

void NetworkClient::stopAsioThread() {
    m_stopping = true;
    m_eventProcessTimer.stop();
    m_reconnectTimer.stop();

    m_work.reset();

    if (m_thread.joinable()) {
        m_thread.join();
    }

    boost::system::error_code ec;
    if (m_socket->is_open()) {
        m_socket->close(ec);
    }
}
