#ifndef BES_NET_SOCKET_STREAM_H
#define BES_NET_SOCKET_STREAM_H

#include <atomic>
#include <functional>
#include <thread>

#include "socket.h"

namespace bes::net::socket {

class Stream : public Socket
{
   public:
    Stream() = default;
    virtual ~Stream();
    Stream(Stream&& s);
    Stream& operator=(Stream&& s);

    /**
     * Listen for connections before opening a new socket to process.
     *
     * @param max_queue Number of connections to queue waiting before rejecting
     * @param tv_sec Number of seconds before polling for a kill signal
     * @param tv_usec Number of microseconds before polling for a kill signal
     */
    void Listen(std::function<void(Stream&&)> const& callback, size_t max_queue = 5, long tv_sec = 1, long tv_usec = 0);
    void ListenAsync(std::function<void(Stream&&)> const& callback, size_t max_queue = 5, long tv_sec = 1,
                     long tv_usec = 0);

    /**
     * Send a kill-signal to the Listen() function, allowing it to drop out when the timeout is next reached.
     *
     * If `wait` is true and the listen loop was executed with ListenAsync(), Stop() will block until the listen loop
     * has completely shutdown.
     */
    void Stop(bool wait = true);

    /**
     * Read `len` bytes from the stream.
     */
    void ReadBytes(void const* buf, size_t len);

    /**
     * Write `len` bytes to the stream.
     */
    void WriteBytes(void const* buf, size_t len);

   protected:
    socket_opt_t GetSocketOptions() override;

   private:
    Stream(int s);
    void Move(Stream&& s);

    std::thread listen_thread;
    std::atomic<bool> listening{false};
    std::atomic<bool> kill_signal{false};
};

}  // namespace bes::net::socket

#endif
