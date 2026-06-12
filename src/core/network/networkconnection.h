#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <deque>


class Logger;

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = asio::ip::tcp;


class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    struct Observer {
        virtual void onReceived(int connectionId, const char* data, size_t size);
        virtual void onConnectionClosed(int connectionId);
    };

    static std::shared_ptr<TcpConnection> create(
        boost::asio::ip::tcp::socket &&socket, Observer& observer, int id = 0);

    void startReading();
    void send(const char* data, size_t size);
    void close();

private:
    TcpConnection(boost::asio::ip::tcp::socket &&socket, Observer& observer,
                  int id);
    void doRead();
    void doWrite();

private:
    std::shared_ptr<Logger> m_log;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_readBuffer;
    boost::asio::streambuf m_writeBuffer;
    std::mutex m_writeBufferMutex;
    Observer& m_observer;
    bool m_isReading;
    bool m_isWritting;
    int m_id;
};

class TcpClient : private TcpConnection::Observer {
public:
    struct Observer {
        virtual void onConnected();
        virtual void onReceived(const char* data, size_t size);
        virtual void onDisconnected();
    };

    TcpClient(boost::asio::io_context& ioContext, Observer& observer);

    void connect(const boost::asio::ip::tcp::endpoint& endpoint);
    void send(const char* data, size_t size);
    void disconnect();

private:
    void onReceived(int connectionId, const char* data, size_t size) override;
    void onConnectionClosed(int connectionId) override;

private:
    std::shared_ptr<Logger> m_log;
    boost::asio::io_context& m_ioContext;
    std::shared_ptr<TcpConnection> m_connection;
    Observer& m_observer;
};

#endif // NETWORKCLIENT_H
