#include "networkconnection.h"

#include "../../utils/logger.h"


namespace {
constexpr size_t f_readBufferSize{1024};
} // namespace

void TcpConnection::Observer::onReceived([[maybe_unused]] int connectionId,
                                         [[maybe_unused]] const char* data,
                                         [[maybe_unused]] const size_t size) {}

void TcpConnection::Observer::onConnectionClosed(
    [[maybe_unused]] int connectionId) {}

TcpConnection::TcpConnection(boost::asio::ip::tcp::socket&& socket,
                             Observer& observer, int id)
    : m_socket{std::move(socket)}, m_readBuffer{}, m_writeBuffer{},
    m_writeBufferMutex{}, m_observer{observer}, m_isReading{false},
    m_isWritting{false}, m_id{id} {}

std::shared_ptr<TcpConnection>
TcpConnection::create(boost::asio::ip::tcp::socket&& socket, Observer& observer,
                      int id) {
    return std::shared_ptr<TcpConnection>(
        new TcpConnection{std::move(socket), observer, id});
}

void TcpConnection::startReading() {
    if (!m_isReading) {
        doRead();
    }
}

void TcpConnection::send(const char* data, size_t size) {
    std::lock_guard<std::mutex> guard{m_writeBufferMutex};
    std::ostream bufferStream{&m_writeBuffer};
    bufferStream.write(data, size);
    if (!m_isWritting) {
        doWrite();
    }
}

void TcpConnection::close() {
    try {
        m_socket.cancel();
        m_socket.close();
    } catch (const std::exception& e) {
        m_log->error("TcpConnection::close() exception: ",
                     static_cast<std::string>(e.what()));
        return;
    }
    m_observer.onConnectionClosed(m_id);
}

void TcpConnection::doRead() {
    m_isReading = true;
    auto buffers{m_readBuffer.prepare(f_readBufferSize)};
    auto self{shared_from_this()};
    m_socket.async_read_some(buffers, [this, self](const auto& error,
                                                   auto bytesTransferred) {
        if (error) {
            m_log->error("TcpConnection::doRead() error: ", error.message());
            return close();
        }
        m_readBuffer.commit(bytesTransferred);
        m_observer.onReceived(m_id,
                              static_cast<const char*>(m_readBuffer.data().data()),
                              bytesTransferred);
        m_readBuffer.consume(bytesTransferred);
        doRead();
    });
}

void TcpConnection::doWrite() {
    m_isWritting = true;
    auto self{shared_from_this()};
    m_socket.async_write_some(m_writeBuffer.data(), [this, self](
                                                        const auto& error,
                                                        auto bytesTransferred) {
        if (error) {
            m_log->error("TcpConnection::doWrite() error: ", error.message());
            return close();
        }
        std::lock_guard<std::mutex> guard{m_writeBufferMutex};
        m_writeBuffer.consume(bytesTransferred);
        if (m_writeBuffer.size() == 0) {
            m_isWritting = false;
            return;
        }
        doWrite();
    });
}

void TcpClient::Observer::onConnected() {}

void TcpClient::Observer::onReceived([[maybe_unused]] const char* data,
                                     [[maybe_unused]] size_t size) {}

void TcpClient::Observer::onDisconnected() {}

TcpClient::TcpClient(boost::asio::io_context& ioContext, Observer& observer)
    : m_ioContext{ioContext}, m_connection{}, m_observer{observer} {}

void TcpClient::connect(const boost::asio::ip::tcp::endpoint& endpoint) {
    if (m_connection) {
        return;
    }
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(m_ioContext);
    socket->async_connect(endpoint, [this, socket](const auto& error) {
        if (error) {
            m_log->error("TcpClient::connect() error: ", error.message());
            m_observer.onDisconnected();
            return;
        }
        m_connection = TcpConnection::create(std::move(*socket), *this);
        m_connection->startReading();
        m_log->info("TCPClient connected");
        m_observer.onConnected();
    });
}

void TcpClient::send(const char* data, size_t size) {
    if (!m_connection) {
        m_log->error("TcpClient::send() error: no connection");
        return;
    }
    m_connection->send(data, size);
}

void TcpClient::disconnect() {
    if (m_connection) {
        m_connection->close();
    }
}

void TcpClient::onReceived([[maybe_unused]] int connectionId, const char* data,
                           size_t size) {
    m_observer.onReceived(data, size);
}

void TcpClient::onConnectionClosed([[maybe_unused]] int connectionId) {
    if (m_connection) {
        m_connection.reset();
        m_log->info("TCPClient disconnected");
        m_observer.onDisconnected();
    }
}
