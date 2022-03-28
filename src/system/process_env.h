#ifndef PROCESS_ENV_H
#define PROCESS_ENV_H

// exit() getenv() setenv
#include <cstdlib>
// _exit()
#include <unistd.h>

// getrlimit() setrlimit()
#include <sys/resource.h>

// std::underlying_type<>
#include <type_traits>

// std::make_tuple
#include <tuple>

#include "general/inc_exception.h"
#include <string>
#include <vector>

namespace ENV {

// invoke exit handlers(if some), then do some clean up
inline void exit(int status) { return ::exit(status); }
// register exit handlers, up to 32
void atexit(void (*func)(void)) {
  if (::atexit(func) != 0)
    throw std::runtime_error("atexit() register failed");
}
// jump into kernel immediately
inline void _exit(int status) { return ::_exit(status); }

std::vector<std::string> getArg(int const argc, char const *argv[]) {
  std::vector<std::string> res{};
  for (int i = 0; i < argc; i++) {
    res.push_back(std::string(argv[i]));
  }
  return res;
}

std::string getEnv(std::string const &key) {
  auto p = ::getenv(key.c_str());
  if (p == nullptr)
    throw std::range_error("Env not found: " + key);
  return std::string(p);
}

void setEnv(std::string const &key, std::string const &value) {
  if (::setenv(key.c_str(), value.c_str(), true) == -1)
    throw std::range_error("Env set failed: " + key + "=" + value);
}

void DelEnv(std::string const &key) {
  if (unsetenv(key.c_str()) == -1)
    throw std::range_error("Env delete failed: " + key);
}

enum class RLimit : int {
  // Available Storage per Process
  AvailableStorage_Max_Proc = RLIMIT_AS,
  // Coredump Size Max for User
  Coredump_Max = RLIMIT_CORE,
  // if excceed, will get SIGXCPI
  CPUTime_Max_Proc = RLIMIT_CPU,
  // Data Segment Max Size per Process
  DataSeg_Max_Proc = RLIMIT_DATA,

  // Max File to be created, if excceed, will get SIGXFSZ
  File_Max_Proc = RLIMIT_FSIZE,
  // Max Storage Size to be mlock()-ed
  Memlock_Max_Proc = RLIMIT_MEMLOCK,
  // Posix Message Queue Max Storage
  MsgQueue_Max_Proc = RLIMIT_MSGQUEUE,
  // Nice Max Limit
  Nice_Max_Proc = RLIMIT_NICE,
  // Max Number of File to be opened per Process
  FileNum_Max_Proc = RLIMIT_NOFILE,
  // Max Child Process for a User
  ProcNum = RLIMIT_NPROC,
  // Resident set size in bytes Max Size, if excceed, kernel will swap
  // it to external storage
  ResidentSetSize_Max_Proc = RLIMIT_RSS,
  // Signal Pending Max Length
  SigPending_Max_Proc = RLIMIT_SIGPENDING,
  // Max Stack Size
  StackSize_Max_Proc = RLIMIT_STACK,
};

auto const RLimitInfinit = RLIM_INFINITY;

using RLimitType = std::underlying_type<RLimit>::type;
inline auto getRLimit(RLimit resource) {
  struct rlimit tmp {};
  if (getrlimit(static_cast<RLimitType>(resource), &tmp) != 0)
    throw std::runtime_error(strerror(errno));
  return std::make_tuple(tmp.rlim_cur, tmp.rlim_max);
}

inline void setRLimit(RLimit resource, rlim_t current, rlim_t max) {
  struct rlimit tmp {
    current, max
  };
  if (setrlimit(static_cast<RLimitType>(resource), &tmp) != 0)
    throw std::runtime_error(strerror(errno));
}

} // namespace ENV

#endif
