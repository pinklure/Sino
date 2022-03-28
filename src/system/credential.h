#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include "general/inc_exception.h"

#include <tuple>
// Creadential is a wrap for r e s ID utilities
#include <unistd.h>

namespace Creadential {

// return curren login name
std::string get_LoginName() {
  char *name{nullptr};
  if ((name = getlogin()) == nullptr)
    throw std::runtime_error(strerror(errno));
  return std::string(name);
}

// return Real User ID
inline uid_t get_UID() noexcept { return getuid(); }
// return Effective User ID
inline uid_t get_E_UID() noexcept { return geteuid(); }

// return (real UID, effective UID, saved set-UID)
auto get_RES_UID() {
  uid_t ruid, euid, suid;
  if (getresuid(&ruid, &euid, &suid) == -1)
    throw std::out_of_range(strerror(errno));
  return std::make_tuple(ruid, euid, suid);
}
// for root: uid(arbitrary) ==> real, effective, saved set-user-ID
// for others: uid(real/saved set-user-ID) ==> effective user ID
// -1 for no change
void set_UID(uid_t uid) {
  if (setuid(uid) == -1)
    throw std::runtime_error(strerror(errno));
}
// for root: euid(arbitrary) ==> effective user ID
// for others: euid(real/saved set-user-ID) ==> effective user ID
// -1 for no change
void set_E_UID(uid_t euid) {
  if (seteuid(euid) == -1)
    throw std::runtime_error(strerror(errno));
}

// for root: ruid(arbitrary) ==> real user ID;
//           euid(arbitrary) ==> effective, saved set-user-ID;
// for others: ruid(real/effective user ID) ==> real user ID
//             euid(real/effective/saved set-user-ID) ==> real user ID
// -1 for no change
void set_RE_UID(uid_t ruid, uid_t euid) {
  if (setreuid(ruid, euid) == -1)
    throw std::runtime_error(strerror(errno));
}

// for root: ruid/euid/suid(arbitrary)
//           ==> real, effective, saved set-user-ID
// for others: ruid/euid/suid(read/effective/saved set-user-ID)
//             ==> real, effective, saved set-user-ID
// -1 for no change
void set_RES_UID(uid_t ruid, uid_t euid, uid_t suid) {
  if (setresuid(ruid, euid, suid) == -1)
    throw std::runtime_error(strerror(errno));
}

inline gid_t get_GID() { return getgid(); }
inline gid_t get_E_GID() { return getegid(); }
auto get_RES_GID() {
  gid_t rgid, egid, sgid;
  if (getresgid(&rgid, &egid, &sgid) == -1)
    throw std::out_of_range(strerror(errno));
  return std::make_tuple(rgid, egid, sgid);
}

void set_GID(uid_t gid) {
  if (setgid(gid) == -1)
    throw std::runtime_error(strerror(errno));
}

void set_E_GID(uid_t egid) {
  if (setegid(egid) == -1)
    throw std::runtime_error(strerror(errno));
}

void set_RE_GID(uid_t rgid, uid_t egid) {
  if (setregid(rgid, egid) == -1)
    throw std::runtime_error(strerror(errno));
}

void set_RES_GID(uid_t rgid, uid_t egid, uid_t sgid) {
  if (setresgid(rgid, egid, sgid) == -1)
    throw std::runtime_error(strerror(errno));
}

} // namespace Creadential

#endif
