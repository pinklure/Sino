#ifndef MULTIPLEX_H
#define MULTIPLEX_H
// 封装 select, poll, epoll 及其拓展性函数

#include <chrono>
#include <functional>
#include <unordered_map>
/* #include <vector> */

#include "system/file_descriptor.h"

using std::chrono_literals::operator""ms;

namespace IOMUL {

/* using FdEvent = struct FdEvent { */
/*   int _fd; */
/*   // callback could be using conditional_variable or something,
 */
/*   // whatever */
/*   std::function<void()> _read_callback{}; */
/*   std::function<void()> _write_callback{}; */
/*   std::function<void()> _error_callback{}; */
/* }; */

class Multiplex {
protected:
  using CallBack = struct CallBack {
    std::function<void()> _read_callback{};
    std::function<void()> _write_callback{};
    std::function<void()> _error_callback{};
  };

  void Register(int fd, CallBack &&callback) {
    _registered[fd] = std::move(callback);
  }

protected:
  std::unordered_map<int, CallBack> _registered{};

public:
  virtual void Register(
      int fd,
      decltype(CallBack::_read_callback) read_callback = nullptr,
      decltype(CallBack::_write_callback) write_callback = nullptr,
      decltype(CallBack::_error_callback) error_callback =
          nullptr) {
    Register(fd, CallBack{read_callback, write_callback,
                          error_callback});
  }

  // if no match, ignore it silently
  virtual void Unregister(int fd, bool read = false,
                          bool write = false, bool error = false) {
    auto it = _registered.find(fd);
    if (it != _registered.end()) {
      if (read)
        it->second._read_callback = nullptr;
      if (write)
        it->second._write_callback = nullptr;
      if (error)
        it->second._error_callback = nullptr;

      if (it->second._read_callback == nullptr &&
          it->second._write_callback == nullptr &&
          it->second._write_callback == nullptr)
        _registered.erase(it);
    }
  }

  // 注册fd与对应event
  // wait infinitely, if some event happens, invoke callback
  virtual void Wait() = 0;
  // wait for giventime, if some event happens, invoke callback
  virtual void Wait(std::chrono::milliseconds ms) = 0;
};

} // namespace IOMUL

#endif
