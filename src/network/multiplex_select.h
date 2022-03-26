#ifndef MULTIPLEX_SELECT_H
#define MULTIPLEX_SELECT_H

#include "network/multiplex.h"

#include <sys/select.h>

namespace IOMUL {

// 当前只完成了 select 版本，搞定信号处理之后再修改为 pselect 版本
class Select final : public Multiplex {
private:
  fd_set read_sets[1];
  fd_set write_sets[1];
  fd_set error_sets[1];

private:
  // 从 _registered 将fd添加到 fd_sets 中
  void PrepareSets() {
    FD_ZERO(read_sets);
    FD_ZERO(write_sets);
    FD_ZERO(error_sets);

    for (auto &it : _registered) {
      if (it.second._read_callback != nullptr)
        FD_SET(it.first, read_sets);
      if (it.second._write_callback != nullptr)
        FD_SET(it.first, write_sets);
      if (it.second._error_callback != nullptr)
        FD_SET(it.first, error_sets);
    }
  }

  void InvokeCallback(int ret) {
    if (ret == -1)
      throw std::runtime_error(strerror(errno));
    for (int i = 0; i < ret; i++) {
      if (FD_ISSET(i, read_sets)) {
        _registered[i]._read_callback();
      }
      if (FD_ISSET(i, write_sets)) {
        _registered[i]._write_callback();
      }
      if (FD_ISSET(i, error_sets)) {
        _registered[i]._error_callback();
      }
    }
  }

public:
  void Register(
      int fd,
      decltype(CallBack::_read_callback) read_callback = nullptr,
      decltype(CallBack::_write_callback) write_callback = nullptr,
      decltype(CallBack::_error_callback) error_callback =
          nullptr) override {
    if (fd >= FD_SETSIZE || fd < 0)
      throw std::range_error(
          "fd's value should be in [0, FD_SETSIZE-1]");
    Multiplex::Register(fd, read_callback, write_callback,
                        error_callback);
  }

  void Unregister(int fd, bool read = false, bool write = false,
                  bool error = false) override {
    Multiplex::Unregister(fd, read, write);

    if (read)
      FD_CLR(fd, read_sets);
    if (write)
      FD_CLR(fd, write_sets);
    if (error)
      FD_CLR(fd, error_sets);

    if (!FD_ISSET(fd, read_sets) && !FD_ISSET(fd, write_sets) &&
        !FD_ISSET(fd, error_sets))
      FD_CLR(fd, error_sets);
  }

  void Wait() override {
    PrepareSets();
    InvokeCallback(select(static_cast<int>(_registered.size()) + 1,
                          read_sets, write_sets, error_sets,
                          nullptr));
  }

  void Wait(std::chrono::milliseconds timeout_ms) override {
    auto timeout_s =
        std::chrono::duration_cast<std::chrono::seconds>(
            timeout_ms);
    struct timeval timeout {
      timeout_s.count(), timeout_ms.count()
    };
    InvokeCallback(select(static_cast<int>(_registered.size()) + 1,
                          read_sets, write_sets, error_sets,
                          &timeout));
  }
};

} // namespace IOMUL

#endif
