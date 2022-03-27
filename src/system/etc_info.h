#ifndef ETC_INFO_H
#define ETC_INFO_H

// getpwdnam() getpwuid()
#include <pwd.h>

#include "general/inc_exception.h"

namespace ETC {

// if userid or username is defaulted, then return current user info
class Passwd final {
 private:
  std::string _username{};
  std::string _password{};
  std::string _userinfo{};
  std::string _homedir{};
  std::string _shell{};
  uid_t _userid{};
  gid_t _groupid{};
  bool _filechanged{true};

 private:
  void reloadPasswd();

 public:
  std::string getUsername();
  std::string getUsername(uid_t userid);
  uid_t getUserId();
  uid_t getUserId(std::string username = {});

  std::string getPassword(uid_t userid);
  std::string getPassword(std::string username = {});
  std::string getUserinfo(uid_t userid);
  std::string getUserinfo(std::string username = {});
  std::string getHomedir(uid_t userid);
  std::string getHomedir(std::string username = {});
  std::string getShell(uid_t userid);
  std::string getShell(std::string username = {});
  gid_t getGroupId(uid_t userid);
  gid_t getGroupId(std::string username = {});
};

inline std::string Passwd::getUsername() {
	
}
inline std::string Passwd::getUsername(uid_t userid) {}
inline uid_t getUserId() {}
inline uid_t getUserId(std::string username) {}

inline std::string getPassword(uid_t userid) {}
inline std::string getPassword(std::string username) {}
inline std::string getUserinfo(uid_t userid) {}
inline std::string getUserinfo(std::string username) {}
inline std::string getHomedir(uid_t userid) {}
inline std::string getHomedir(std::string username) {}
inline std::string getShell(uid_t userid) {}
inline std::string getShell(std::string username) {}
inline gid_t getGroupId(uid_t userid) {}
inline gid_t getGroupId(std::string username) {}

}  // namespace ETC

#endif
