#ifndef TCP_WRAP_H
#define TCP_WRAP_H

#include "fd_wrap.h"
//#include <sys/types.h> // Linux Don't Use It
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_pton
#include <memory>
#include <signal.h>
#include <regex>

class TCP {
private:
    class SocketAddress {
    private:
        struct sockaddr_in _addr;
    public:
        auto Address2Sockaddr(std::string const &ip, uint16_t port) noexcept {
            bzero(&_addr, sizeof _addr);
            _addr.sin_family = AF_INET;
            _addr.sin_port = htons(port);
            _addr.sin_addr.s_addr = inet_addr(ip.c_str());
            return reinterpret_cast<struct sockaddr const *>(&_addr);
        }

        auto Fd2Sockaddr(FD const &fd) {
            if (getsockname(fd.Get(), reinterpret_cast<struct sockaddr *>(&_addr), &Size()) == -1)
                throw std::runtime_error(strerror(errno));
            return reinterpret_cast<struct sockaddr const *>(&_addr);
        }

        auto Fd2Address(FD const &fd) {
            auto p = reinterpret_cast<struct sockaddr_in const *>(Fd2Sockaddr(fd));
            std::string res(inet_ntoa(p->sin_addr));
            uint16_t port = ntohs(p->sin_port);
            return std::make_pair(res, port);
        }

        inline unsigned int &Size() noexcept {
            static socklen_t _size{sizeof(_addr)};
            return _size;
        }
    };

private:
    std::unique_ptr<FD> _fd{nullptr};
//    std::unique_ptr<std::vector<FD>> _peer_list{std::make_unique<std::vector<FD>>()};
    SocketAddress _SockAddress;
    static std::regex const ipv4_pattern;
public:
    TCP(std::string const &addr = {}, uint16_t port = {}) {
        int fd = socket(AF_INET, SOCK_STREAM, 6);
        if (fd == -1)
            throw std::runtime_error(strerror(errno));
        _fd = std::make_unique<FD>(fd);

        if (!addr.empty()) {
            if (!std::regex_match(addr, ipv4_pattern) || port == 0)
                throw std::runtime_error("Ipv4 Address Or Port Number Not Valid: " + addr + ":" + std::to_string(port));
            if (bind(_fd->Get(), _SockAddress.Address2Sockaddr(addr, port), _SockAddress.Size()) == -1)
                throw std::runtime_error(strerror(errno));
        }
    }

    void Connect(std::string const &addr = {}, uint16_t port = {}) {
        if (!std::regex_match(addr, ipv4_pattern) || port == 0)
            throw std::runtime_error("Ipv4 Address Or Port Number Not Valid: " + addr + ":" + std::to_string(port));
        if (connect(_fd->Get(), _SockAddress.Address2Sockaddr(addr, port), _SockAddress.Size()) == -1)
            throw std::runtime_error(strerror(errno));
    }

    void Listen(int backlog = 256) {
        if (listen(_fd->Get(), backlog) == -1)
            throw std::runtime_error(strerror(errno));
    }

//    void Accept(bool unblocking = false, std::string const& addr={})
    [[nodiscard]] FD Accept(std::string const &addr = {}) {
        int fd = 0;
        if (addr.empty()) {
            fd = accept(_fd->Get(), nullptr, nullptr);
        } else {
            if (!std::regex_match(addr, ipv4_pattern))
                throw std::runtime_error("Ipv4 Address Not Valid: " + addr);
            socklen_t sz = _SockAddress.Size();
            fd = accept(_fd->Get(), const_cast<struct sockaddr *>(_SockAddress.Address2Sockaddr(addr, 0)), &sz);
        }
        if (fd == -1)
            throw std::runtime_error(strerror(errno));
        return FD(fd);
//        _peer_list->push_back(FD(fd));
    }

    std::pair<std::string, uint16_t> GetAddress() {
        return GetAddress(*_fd.get());
    }

    std::pair<std::string, uint16_t> GetAddress(FD const &fd) {
        return _SockAddress.Fd2Address(fd);
    }

    // offset is often used when last Write did not send all data, which should reduce memory copy and write
    ssize_t Write(std::vector<uint8_t> const &data, FD const &peer, std::size_t offset = 0) {
        auto sz = data.size();
        auto n = write(peer.Get(), data.data(), data.size());
        if (n == -1)
            throw std::runtime_error(strerror(errno));
        return n;
    }

    ssize_t Write(std::vector<uint8_t> const &data, std::size_t offset = 0) {
        return Write(data , *_fd.get(), offset);
    }

    std::vector<uint8_t> Read(ssize_t sz, FD const &peer) {
        std::vector<uint8_t> res(sz);
        auto n = read(peer.Get(), res.data(), sz);
        if (n == -1)
            throw std::runtime_error(strerror(errno));
        return res;
    }

    std::vector<uint8_t> Read(ssize_t sz) {
        return Read(sz, *_fd.get());
    }

    void SetSigpipeHandler(void (*handler)(int)) {
        struct sigaction sig{handler};
        if (sigaction(SIGPIPE, &sig, nullptr) == -1) {
            throw std::runtime_error(strerror(errno));
        }
    }

};

std::regex const TCP::ipv4_pattern("^(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}$");

#endif //SIHTTP_TCP_WRAP_H
