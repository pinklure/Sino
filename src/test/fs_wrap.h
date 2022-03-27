#ifndef FS_WRAP_H
#define FS_WRAP_H

#include "fd_wrap.h"
#include <sys/stat.h>
#include <error.h>
#include <dirent.h>

#include <string>
#include <vector>


class FS {
private:


public:

    // Flags Range
    // O_RDONLY Open for reading only
    // O_WRONLY Open for writing only
    // O_RDWR Open for reading and writing
    // O_CLOEXEC Set the close-on-exec flag (since Linux 2.6.23)
    // O_CREAT Create file if it doesn’t already exist
    // O_DIRECT File I/O bypasses buffer cache
    // O_DIRECTORY Fail if pathname is not a directory
    // O_EXCL With O_CREAT: create file exclusively
    // O_LARGEFILE Used on 32-bit systems to open large files
    // O_NOATIME Don’t update file last access time on read() (since Linux 2.6.8)
    // O_NOCTTY Don’t let pathname become the controlling terminal
    // O_NOFOLLOW Don’t dereference symbolic links
    // O_TRUNC Truncate existing file to zero length
    // O_APPEND Writes are always appended to end of file
    // O_ASYNC Generate a signal when I/O is possible
    // O_DSYNC Provide synchronized I/O data integrity (since Linux 2.6.33)
    // O_NONBLOCK Open in nonblocking mode
    // O_SYNC Make file writes synchronous
    static FD Open(std::string const &path, int flags) {
        int fd = open(path.c_str(), flags);
        if (fd == -1) {
            throw std::runtime_error(strerror(errno));
        }
        FD tmp(fd);
        return tmp;
    }

    // Constant Octal value Permission bit
    // S_ISUID 04000 Set-user-ID
    // S_ISGID 02000 Set-group-ID
    // S_ISVTX 01000 Sticky
    // S_IRUSR 0400 User-read
    // S_IWUSR 0200 User-write
    // S_IXUSR 0100 User-execute
    // S_IRGRP 040 Group-read
    // S_IWGRP 020 Group-write
    // S_IXGRP 010 Group-execute
    // S_IROTH 04 Other-read
    // S_IWOTH 02 Other-write
    // S_IXOTH 01 Other-execute
    static void Create(std::string const &path, int mode) {
        int res = open(path.c_str(), O_CREAT, mode);
        if (res == -1)
            throw std::runtime_error(strerror(errno));
        if (close(res) == -1)
            throw std::runtime_error(strerror(errno));
    }


    static ssize_t Read(FD &fd, std::vector<char> &buf, std::size_t size) {
        buf.resize(size);
        ssize_t sz = read(fd.Get(), buf.data(), buf.size());
        if (sz == -1) {
            throw std::runtime_error(strerror(errno));
        }
        return sz;
    }

    static void Write(FD &fd, std::vector<char> &buf) {
        std::size_t sz = buf.size();
        ssize_t n = 0;
        while (n != sz) {
            auto res = write(fd.Get(), buf.data() + n, sz - n);
            if (res == -1)
                throw std::runtime_error(strerror(errno));
            n += res;
        }
    }

    static void Write(FD &fd, std::string &buf) {
        std::size_t sz = buf.size();
        ssize_t n = 0;
        while (n != sz) {
            auto res = write(fd.Get(), buf.c_str() + n, sz - n);
            if (res == -1)
                throw std::runtime_error(strerror(errno));
            n += res;
        }
    }

    static void Close(FD &fd) {
        int res = close(fd.Get());
        if (res == -1)
            throw std::runtime_error(strerror(errno));
    }

    // from available value
    // SEEK_SET begining of the file
    // SEEK_CUR relative to current file offset
    // SEEK_END from the end
    static std::size_t GetOffset(FD &fd, int from) {
        auto res = lseek(fd.Get(), 0, from);
        if (res == -1)
            throw std::runtime_error(strerror(errno));
        return res;
    }

    static std::size_t SetOffset(FD &fd, long offset, int from) {
        auto res = lseek(fd.Get(), offset, from);
        if (res == -1)
            throw std::runtime_error(strerror(errno));
        return res;
    }


    class FileState {
    private:
        struct stat _state{};
        bool _valid{false};

        void CheckIfValid() const {
            if (!_valid) {
                throw std::runtime_error("File State has not been Got");
            }
        }

    public:
        void operator()(std::string const &path) {
            if (stat(path.c_str(), &_state) == -1) {
                _valid = false;
                throw std::runtime_error(strerror(errno));
            }
            _valid = true;
        }

        void operator()(FD const &fd) {
            if (fstat(fd.Get(), &_state) == -1) {
                _valid = false;
                throw std::runtime_error(strerror(errno));
            }
            _valid = true;
        }

        struct stat const &Get() {
            CheckIfValid();
            return _state;
        }

        time_t ModifiedTime() const {
            CheckIfValid();
            return _state.st_mtim.tv_sec;
        }

        time_t AccessedTime() const {
            CheckIfValid();
            return _state.st_atim.tv_sec;
        }

        time_t ChangedTime() const {
            CheckIfValid();
            return _state.st_ctim.tv_sec;
        }

        uid_t Owner() const {
            CheckIfValid();
            return _state.st_uid;
        }

        uid_t OwnerGroup() const {
            CheckIfValid();
            return _state.st_gid;
        }

        mode_t FileMode() const {
            CheckIfValid();
            S_ISBLK(_state.st_mode);
            return _state.st_mode;
        }

        bool IsRegularFile() const {
            if (S_ISREG(FileMode()))
                return true;
            else return false;
        };

        bool IsDirectory() const {
            if (S_ISDIR(FileMode()))
                return true;
            else return false;
        };

        bool IsCharactorFile() const {
            if (S_ISCHR(FileMode()))
                return true;
            else return false;
        };

        bool IsBlockFile() const {
            if (S_ISBLK(FileMode()))
                return true;
            else return false;
        };

        bool IsFifoOrPipe() const {
            if (S_ISFIFO(FileMode()))
                return true;
            else return false;
        };

        bool IsSoftLink() const {
            if (S_ISLNK(FileMode()))
                return true;
            else return false;
        };

        bool IsSocket() const {
            if (S_ISSOCK(FileMode()))
                return true;
            else return false;
        };

        bool IsMessageQueue() const {
            CheckIfValid();
            if (S_TYPEISMQ(&_state))
                return true;
            else return false;
        }

        bool IsSemaphore() const {
            CheckIfValid();
            if (S_TYPEISSEM(&_state))
                return true;
            else return false;
        }

        bool IsSharedMemory() const {
            CheckIfValid();
            if (S_TYPEISSHM(&_state))
                return true;
            else return false;
        }

        // only available for Regular File, Directory, Soft Link
        off_t FileSize() const {
            if (IsRegularFile() || IsDirectory() || IsSoftLink())
                return _state.st_size;
            else
                throw std::runtime_error("File Size is only available"
                                         " for Regular File, Directory, Soft Link");
        }

    };


    // -1 for no change
    static void Chown(std::string const &path, uid_t owner = -1, gid_t group = -1) {
        if (chown(path.c_str(), owner, group) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static void Chown(FD const &fd, uid_t owner = -1, gid_t group = -1) {
        if (fchown(fd.Get(), owner, group) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static void Chmod(std::string const &path, mode_t mode) {
        if (chmod(path.c_str(), mode) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static void Chmod(FD const &fd, mode_t mode) {
        if (fchmod(fd.Get(), mode) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static bool Readable(std::string const &path) {
        return !access(path.c_str(), R_OK);
    }

    static bool Writeable(std::string const &path) {
        return !access(path.c_str(), W_OK);
    }

    static bool Executable(std::string const &path) {
        return !access(path.c_str(), X_OK);
    }

    static void Makedir(std::string const &path, mode_t const mode) {
        if (mkdir(path.c_str(), mode) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static void RemoveDir(std::string const &path) {
        if (rmdir(path.c_str()) == -1)
            throw std::runtime_error(strerror(errno));
    }

    using Directory = struct Directory {
        unsigned long inode{};
        std::string name{};

        Directory(unsigned long i, std::string &&n)
                : inode{i}, name{std::move(n)} {}
    };

    // not finish
    static std::vector<Directory> ReadDirectory(std::string const &path) {
        auto dir = opendir(path.c_str());
        if (dir == nullptr)
            throw std::runtime_error(strerror(errno));

        errno = 0;
        std::vector<Directory> res{};
        dirent *entry{nullptr};
        while ((entry = readdir(dir)) != nullptr) {
            res.emplace_back(entry->d_ino, entry->d_name);
        }
        if (errno != 0) {
            throw std::runtime_error(strerror(errno));
        }
        return res;
    }

    static void Chdir(std::string const &path) {
        if (chdir(path.c_str()) == -1)
            throw std::runtime_error(strerror(errno));
    }

    static void Chdir(FD const &fd) {
        if (fchdir(fd.Get()) == -1)
            throw std::runtime_error(strerror(errno));
    }

    // problematic
    static void GetCWD(std::vector<char> vec) {
        vec.resize(100);
        if (getcwd(vec.data(), vec.size()) == nullptr)
            throw std::runtime_error(strerror(errno));
    }

    // rename or move
    static void Move(std::string const &oldname, std::string const &newname) {
        if (rename(oldname.c_str(), newname.c_str()) == -1)
            throw std::runtime_error(strerror(errno));
    }
};


#endif //SIHTTP_FS_WRAP_H
