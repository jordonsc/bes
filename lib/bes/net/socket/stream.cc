#include "stream.h"

using namespace bes::net::socket;

Stream::Stream(int s)
{
    sock = s;
    is_bound.store(true);
    is_open.store(true);
}

Stream::Stream(Stream&& s) noexcept
{
    move(std::move(s));
}

Stream& Stream::operator=(Stream&& s) noexcept
{
    move(std::move(s));
    return *this;
}

void Stream::move(Stream&& s)
{
    using std::swap;

    sock = s.sock;
    sock_addr = s.sock_addr;
    std::swap(listen_thread, s.listen_thread);
    listening.store(s.listening);
    kill_signal.store(s.kill_signal);
    is_bound.store(s.is_bound);
    is_open.store(s.is_open);

    s.sock = 0;
    s.is_open.store(false);
    s.is_bound.store(false);
    s.listening.store(false);
    s.kill_signal.store(false);
}

socket_opt_t Stream::getSocketOptions()
{
    return {AF_INET, SOCK_STREAM, 0};
}

void Stream::listen(std::function<void(Stream&&)> const& callback, size_t max_queue_len, long tv_sec, long tv_usec)
{
    if (listening.load()) {
        throw SocketException("Already listening for connections");
    }

    listening.store(true);
    ::listen(sock, max_queue_len);

    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    fd_set set;
    struct timeval timeout;

    while (!kill_signal.load()) {
        // Need to reset FD set & timeout each iteration
        FD_ZERO(&set);
        FD_SET(sock, &set);  // Add our main listening socket to the set

        timeout.tv_sec = tv_sec;
        timeout.tv_usec = tv_usec;

        // Select will wait for a new connection and return 0 if no new connections are available, or -1 on error
        if (::select(sock + 1, &set, NULL, NULL, &timeout) > 0) {
            int sub_socket = ::accept(sock, (struct sockaddr*)&client, (socklen_t*)&addr_size);
            if (sub_socket > 0) {
                callback(Stream(sub_socket));
            }
        }
    }

    listening.store(false);
}

void Stream::listenAsync(std::function<void(Stream&&)> const& callback, size_t max_queue_len, long tv_sec, long tv_usec)
{
    if (listen_thread.joinable()) {
        listen_thread.join();
    }

    listen_thread = std::thread([this, callback, max_queue_len, tv_sec, tv_usec] {
        listen(callback, max_queue_len, tv_sec, tv_usec);
    });
}

void Stream::stop(bool wait)
{
    kill_signal.store(true);

    if (wait && listen_thread.joinable()) {
        listen_thread.join();
    }
}

void Stream::readBytes(const void* buf, size_t len)
{
    if (len == 0) {
        return;
    }

    int r;
    size_t read_count = 0;
    do {
        errno = 0;
        r = ::read(sock, ((char*)buf) + read_count, len - read_count);
        if (r == -1) {
            throw SocketException("Socket read failed");
        } else {
            read_count += r;
        }
    } while (read_count != len);
}

void Stream::writeBytes(const void* buf, size_t len)
{
    if (len == 0) {
        return;
    }

    int r;
    size_t write_count = 0;
    do {
        errno = 0;
        r = ::write(sock, ((char*)buf) + write_count, len - write_count);
        if (r == -1) {
            throw SocketException("Socket write failed");
        } else {
            write_count += r;
        }
    } while (write_count != len);
}

Stream::~Stream()
{
    stop();
}
