#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "socket.h"

namespace bes::net::socket {

class Stream : public Socket
{
   public:
    Stream() = default;
    ~Stream() override;

    Stream(Stream&& s) noexcept;
    Stream& operator=(Stream&& s) noexcept;

    /**
     * Listen for connections before opening a new socket to process.
     *
     * @param max_queue Number of connections to queue waiting before rejecting
     * @param tv_sec Number of seconds before polling for a kill signal
     * @param tv_usec Number of microseconds before polling for a kill signal
     */
    void listen(std::function<void(Stream&&)> const& callback, size_t max_queue = 5, long tv_sec = 1, long tv_usec = 0);
    void
    listenAsync(std::function<void(Stream&&)> const& callback, size_t max_queue = 5, long tv_sec = 1, long tv_usec = 0);

    /**
     * Send a kill-signal to the Listen() function, allowing it to drop out when the timeout is next reached.
     *
     * If `wait` is true and the listen loop was executed with ListenAsync(), Stop() will block until the listen loop
     * has completely shutdown.
     */
    void stop(bool wait = true);

    /**
     * Read `len` bytes from the stream.
     */
    void readBytes(void const* buf, size_t len);

    /**
     * Write `len` bytes to the stream.
     */
    void writeBytes(void const* buf, size_t len);

   protected:
    socket_opt_t getSocketOptions() override;

   private:
    explicit Stream(int s);
    void move(Stream&& s);

    std::thread listen_thread;
    std::atomic<bool> listening{false};
    std::atomic<bool> kill_signal{false};
};

}  // namespace bes::net::socket
