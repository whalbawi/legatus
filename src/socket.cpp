#include "axle/socket.h"

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h> // IWYU pragma: keep -- for ssize_t

#include <cerrno>
#include <cstdint>
#include <cstdio>

#include <span>
#include <stdexcept>
#include <string>

#include "axle/status.h"

namespace {

int do_fcntl(int fd, int cmd, int flags) {
    return fcntl(fd, cmd, flags); // NOLINT(cppcoreguidelines-pro-type-vararg, misc-include-cleaner)
}

struct sockaddr* endpoint_to_sockaddr(const std::string& address,
                                      int port,
                                      struct sockaddr_in& addr_in) {
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port); // NOLINT(misc-include-cleaner)
    addr_in.sin_addr.s_addr = inet_addr(address.c_str());

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<struct sockaddr*>(&addr_in);
}

} // namespace

namespace axle {

Socket::Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0)) {
    if (fd_ == -1) {
        throw std::runtime_error("failed to create client socket");
    }
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket::~Socket() {
    (void)close();
}

Status<None, int> Socket::set_non_blocking() const {
    const int flags = do_fcntl(fd_, F_GETFL, 0); // NOLINT(misc-include-cleaner) -- for F_GETFL
    if (flags == -1) {
        perror("failed to get socket flags");

        return Status<None, int>::make_err(errno);
    }

    // NOLINTNEXTLINE(misc-include-cleaner) -- for F_SETFL, O_NONBLOCK
    if (do_fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("failed to put socket in non-blocking mode");

        return Status<None, int>::make_err(errno);
    }

    return Status<None, int>::make_ok();
}

Status<None, int> Socket::send_all(std::span<const uint8_t> buf) const {
    while (!buf.empty()) {
        // NOLINTNEXTLINE(misc-include-cleaner) -- for ssize_t
        const ssize_t len = write(fd_, buf.data(), buf.size());
        if (len == -1) {
            perror("failed to write to socket");

            return Status<None, int>::make_err(errno);
        }
        buf = buf.subspan(len);
    }

    return Status<None, int>::make_ok();
}

Status<std::span<uint8_t>, int> Socket::recv_some(std::span<uint8_t> buf_view) const {
    const ssize_t len = read(fd_, buf_view.data(), buf_view.size());
    if (len == -1) {
        perror("failed to read from connection");

        return Status<std::span<uint8_t>, int>::make_err(errno);
    }

    return Status<std::span<uint8_t>, int>::make_ok(buf_view.first(len));
}

Status<None, int> Socket::close() {
    const int fd = fd_;
    if (fd != -1 && ::close(fd) == -1) {
        perror("failed to close socket fd");

        return Status<None, int>::make_err(errno);
    }

    fd_ = -1;

    return Status<None, int>::make_ok();
}

int Socket::get_fd() const {
    return fd_;
}

Status<None, int> ClientSocket::connect(const std::string& address, int port) const {
    struct sockaddr_in addr_in{};
    struct sockaddr* addr = endpoint_to_sockaddr(address, port, addr_in);

    if (::connect(get_fd(), addr, sizeof(*addr)) == -1) {
        perror("failed to connect to server");

        return Status<None, int>::make_err(errno);
    }

    return Status<None, int>::make_ok();
}

ServerSocket::ServerSocket() {
    int enable = 1;
    if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        throw std::runtime_error("failed to create client socket");
    }
}

Status<None, int> ServerSocket::listen(int port, int backlog) const {
    struct sockaddr_in addr_in{};
    struct sockaddr* addr = endpoint_to_sockaddr("0.0.0.0", port, addr_in);
    if (bind(get_fd(), addr, sizeof(*addr)) == -1) {
        perror("failed to bind to socket");

        return Status<None, int>::make_err(errno);
    }

    if (::listen(get_fd(), backlog) < 0) {
        perror("failed to listen for incoming connections");

        return Status<None, int>::make_err(errno);
    }

    return Status<None, int>::make_ok();
}

Status<Socket, int> ServerSocket::accept() const {
    const int peer_fd = ::accept(get_fd(), nullptr, nullptr);
    if (peer_fd == -1) {
        perror("failed to accept connection");

        return Status<Socket, int>::make_err(errno);
    }

    return Status<Socket, int>::make_ok(Socket(peer_fd));
}

} // namespace axle
