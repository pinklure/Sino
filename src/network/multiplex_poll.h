#ifndef MULTIPLEX_POLL_H
#define MULTIPLEX_POLL_H

#include "network/multiplex.h"
#include <poll.h>

namespace IOMUL {

class Poll final : public Multiplex {
private:
  std::vector<struct pollfd> _fds;

  void PreparePollfd() {
    for (auto &it : _fds)
      it.revents = 0;
  }

  void InvokeCallback(int ret) {
    for (auto &it : _fds) {
      if (ret == 0)
        break;
      if (it.revents > 0) {
        ret--;
        if (it.revents & POLLIN || it.revents & POLLRDHUP ||
            it.revents & POLLPRI)
          _registered[it.fd]._read_callback();
        if (it.revents & POLLOUT)
          _registered[it.fd]._write_callback();
        if (it.revents & POLLERR || it.revents & POLLHUP ||
            it.revents & POLLNVAL)
          _registered[it.fd]._error_callback();
      }
    }
  }

public:
  // POLLERR POLLHUP POLLNVAL will trigger error_callback()
  // POLLRDHUP POLLPRI will trigger read_callback()
  void Register(
      int fd,
      decltype(CallBack::_read_callback) read_callback = nullptr,
      decltype(CallBack::_write_callback) write_callback = nullptr,
      decltype(CallBack::_error_callback) error_callback =
          nullptr) override {

    Multiplex::Register(fd, read_callback, write_callback,
                        error_callback);

    struct pollfd tmp {}; // zero initialize
    tmp.fd = fd;
    if (read_callback != nullptr)
      tmp.events |= POLLIN | POLLRDHUP | POLLPRI;
    if (write_callback != nullptr)
      tmp.events |= POLLOUT;
    if (write_callback != nullptr)
      tmp.events |= POLLERR;
  }

  void Unregister(int fd, bool read = false, bool write = false,
                  bool error = false) override {
    Multiplex::Unregister(fd, read, write, error);

    std::vector<struct pollfd>::iterator it;
    for (; it != _fds.end(); it++) {
      if (it->fd == fd)
        break;
    }
    if (_fds.size() != _registered.size())
      _fds.erase(it); // 说明该fd被删除了
    else {
      if (read)
        it->events &= ~(POLLIN | POLLRDHUP | POLLPRI);
      if (write)
        it->events &= ~(POLLOUT);
      if (error)
        it->events &= ~(POLLERR | POLLHUP | POLLNVAL);
    }
  }

  void Wait() override {
    PreparePollfd();
    int ret = poll(_fds.data(), _fds.size(), -1);
    InvokeCallback(ret);
  }
  void Wait(std::chrono::milliseconds timeout_ms) override {
    auto timeout_s =
        std::chrono::duration_cast<std::chrono::seconds>(
            timeout_ms);
    /* struct timeval timeout { */
    /*   timeout_s.count(), timeout_ms.count() */
    /* }; */
    PreparePollfd();
    int ret = poll(_fds.data(), _fds.size(), timeout_s.count());
    InvokeCallback(ret);
  }
};

} // namespace IOMUL

#endif
