#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QByteArray>
#include <QQueue>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <thread>
#include <atomic>
#include <memory>


namespace asio = boost::asio;
using boost::asio::ip::tcp;

class NetworkClient : public QObject {
    Q_OBJECT

public:
    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Error
    };
    Q_ENUM(ConnectionState)

    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    void connectToServer(const QString &host, int port);
    void disconnect();
    void sendMessage(const QByteArray &data);

    ConnectionState state() const;
    QString errorString() const;

signals:
    void stateChanged(ConnectionState state);
    void messageReceived(const QByteArray &data);
    void errorOccurred(const QString &errorMessage);

private slots:
    void processAsioEvents();
    void attemptReconnect();

private:
    void startReadOperation();
    void handleRead(const boost::system::error_code &error, std::size_t bytesTransferred);
    void handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred);
    void setConnectionState(ConnectionState newState);
    void runAsioThread();
    void stopAsioThread();

private:
    asio::io_context m_io_context;
    std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>> m_work;
    std::shared_ptr<tcp::socket> m_socket;
    std::thread m_thread;
    QTimer m_eventProcessTimer;
    QTimer m_reconnectTimer;

    ConnectionState m_state;
    QString m_errorString;
    QString m_host;
    int m_port;
    std::atomic<bool> m_stopping;

    std::vector<char> m_receiveBuffer;
    QQueue<QByteArray> m_sendQueue;
    std::atomic<bool> m_isWriting;

    static const int RECONNECT_INTERVAL = 5000; // 5 seconds
    static const int EVENT_PROCESS_INTERVAL = 16; // ~60fps
    static const int BUFFER_SIZE = 8192;
};

#endif // NETWORKCLIENT_H
