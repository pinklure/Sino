#ifndef PROCESS_H
#define PROCESS_H

#include "general/inc_exception.h"

// getpid() getppid()
//
#include <unistd.h>

// waitpid()
#include <sys/wait.h>

// system()
#include <cstdlib>

#include <sys/times.h>

#include <tuple>
#include <vector>

namespace PROC {
inline pid_t getPID() noexcept { return ::getpid(); }
inline pid_t getPPID() noexcept { return ::getppid(); }

inline pid_t Fork() {
  auto pid = fork();
  if (pid == -1)
    throw std::runtime_error(strerror(errno));
  return pid;
}
inline pid_t VFork() {
  auto pid = vfork();
  if (pid == -1)
    throw std::runtime_error(strerror(errno));
  return pid;
}

// pid:
//  <-1 wait for any child process whose process group ID is abs(pid)
//  -1  wait for any child process
//  0   wait for any child process whose process group ID is equal to
//        the calling process at the time of call this function
//  >0  wait for the child process whose process ID is equal to pid
// options:
//  WNOHANG : return immediately if no child has exited
//  WUNTRACED : return immediately if a child has stopped
//  WCONTINUED : return immediately if a stopped child has been
//    resumed by SIGCONT
// return (pid, status)
// use following macros to check status
//   WIFEXITED()
//   WEXITSTATUS()
//   WIFSIGNALED()
//   WTERMSIG()
//   WCOREDUMP()
//   WIFSTOPPED()
//   WIFSTOPPED()
//   WSTOPSIG()
//   WIFCONTINUED()
inline auto Wait(pid_t pid = -1, int options = 0) {
  int status{};
  if ((pid = ::waitpid(pid, &status, options)) == -1)
    throw std::runtime_error(strerror(errno));
  return std::make_tuple(pid, status);
}

// to be continue
// waitid() wait3() wait4()

void Exec(std::string &name, std::vector<std::string> &arg,
          std::vector<std::string> &env) {
  // there is no backslash hack
  bool usePATH{true};
  for (auto it : name) {
    if (it == '/') {
      usePATH = false;
      break;
    }
  }
  std::vector<char *> arg_vec(1, name.data());
  for (auto &it : arg)
    arg_vec.push_back(it.data());
  arg_vec.push_back(nullptr);

  std::vector<char *> env_vec{};
  for (auto &it : env)
    env_vec.push_back(it.data());
  env_vec.push_back(nullptr);

  if (usePATH) {
    if (env.empty()) {
      if (execvp(name.c_str(), arg_vec.data()) == -1)
        throw std::runtime_error(strerror(errno));
    } else {
      if (execvpe(name.c_str(), arg_vec.data(), env_vec.data()) == -1)
        throw std::runtime_error(strerror(errno));
    }
  } else {
    if (env.empty()) {
      if (execv(name.c_str(), arg_vec.data()) == -1)
        throw std::runtime_error(strerror(errno));
    } else {
      if (execve(name.c_str(), arg_vec.data(), env_vec.data()) == -1)
        throw std::runtime_error(strerror(errno));
    }
  }
}

// if command.empty(), return none zero if shell is available
// if child process cannot be created or status cannot be retrieved,
//   throw runtime_error
// if shell could not be executed, then return value is as if
//   terminated by calling _exit() with status 127
// if system call
//   succeed, then return shell command termination status
int runShell(std::string const &command) {
  int res = system(command.c_str());
  if (res == -1)
    throw std::runtime_error(strerror(errno));
  return res;
}

int incGetNice(int inc) {
  errno = 0;
  int res = nice(inc);
  if (res == -1 && errno != 0)
    throw std::runtime_error(strerror(errno));
  return res;
}


// to do
/* getpriority(); */
/* setpriority(); */

// weired, havent figure out
/* // return (user time, system time, user time of children, */
/* //   system time of children) */
/* // Times for terminated children (and their descendants) are added
 * in */
/* //   at the moment wait(2) or waitpid(2) returns their process ID.
 * In */
/* //   particular, times of grandchildren that the children did not
 * wait */
/* //   for are never seen. */
/* auto getProcTime() { */
/*   struct tms tmp; */
/*   if (times(&tmp) == -1) */
/*     throw std::runtime_error(strerror(errno)); */
/*   return std::make_tuple(tmp.tms_utime, tmp.tms_stime,
 * tmp.tms_cutime, */
/*                          tmp.tms_cstime); */
/* } */

} // namespace PROC

#endif
