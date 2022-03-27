#ifndef FD_WRAP_H
#define FD_WRAP_H

#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>

class FD {
private:
    int _fd;
    bool _valid;

public:
    int Get() const
    {
        return _fd;
    }

    FD Copy(bool CLOSE_ON_EXEC=false) const
    {
        int cmd = F_DUPFD;
        if(CLOSE_ON_EXEC)
            cmd = F_DUPFD_CLOEXEC;

        int res = fcntl(_fd, cmd);
        if(res==-1)
            throw std::runtime_error(strerror(errno));

        FD tmp(res);
        return tmp;
    }

    int GetFlag() const
    {
        auto res = fcntl(_fd, F_GETFD);
        if(res==-1)
            throw std::runtime_error(strerror(errno));
        return res;
    }

    void SetFlag(int arg) const
    {
        if(fcntl(_fd, F_SETFD, arg)==-1)
            throw std::runtime_error(strerror(errno));
    }

    // what's the difference between F_GETFL F_GETFD ?????
    // don't know....
    int GetStatus() const
    {
        auto res = fcntl(_fd, F_GETFL);
        if(res==-1)
            throw std::runtime_error(strerror(errno));
        return res;
    }

    // Available arg range
    // O_APPEND, O_NONBLOCK, O_SYNC, O_DSYNC, O_RSYNC, O_ASYNC
    void SetStatus(int arg) const
    {
        if(fcntl(_fd, F_SETFL, arg)==-1)
            throw std::runtime_error(strerror(errno));
    }


    pid_t GetAsyncSigOwner() const
    {
        pid_t res = fcntl(_fd, F_GETOWN);
        if(res==-1)
            throw std::runtime_error(strerror(errno));
        return res;
    }

    void SetAsyncSigOwner(pid_t pid) const
    {
        if(fcntl(_fd, F_SETOWN, pid)==-1)
            throw std::runtime_error(strerror(errno));
    }

    // 还有 F_GETLK F_SETLK F_SETLKW 等未包装

public:
    explicit FD(int fd) : _fd(fd), _valid(true) {}
    FD(FD &&obj) noexcept: _fd(obj._fd), _valid(true) {
        obj._fd = -1;
        obj._valid = false;
    }
    FD(FD const &) = delete;
    void operator=(FD const &) = delete;
    FD &operator=(FD &&obj) noexcept {
        if (_fd >= 0) {
            close(_fd);
        }
        _fd = obj._fd;
        _valid = true;
        return *this;
    }
    ~FD() {
        if (_fd >= 0) {
            close(_fd);
        }
    }


};


#endif //SIHTTP_FD_WRAP_H
