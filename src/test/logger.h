#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <type_traits>
#include "fs_wrap.h"


class Logger {
private:
    class LogBuffer {
    private:
        std::queue<std::string> _buffer_main;
        std::queue<std::string> _buffer_back;
        std::mutex _lk_main;
        std::mutex _lk_back;
        std::condition_variable _write_cond;
        std::mutex _write_mutex;
        std::size_t _flush_limit;
        FD _log_fd;
        bool _signal_to_exit{false};


        void swapBuffer() {
            std::unique_lock<std::mutex> lk(_write_mutex);
            _write_mutex.lock();

            std::swap(_buffer_main, _buffer_back);

            _write_cond.notify_one();
            _write_mutex.unlock();
        }

        void writeDisk() {
            std::unique_lock<std::mutex> lk(_write_mutex);
            while (true) {
                _write_mutex.lock();
                _write_cond.wait(lk);
                while (!_buffer_back.empty()) {
                    FS::Write(_log_fd, _buffer_back.front());
                    _buffer_back.pop();
                }
                _write_mutex.unlock();
                if (_signal_to_exit) break;
            }
        }

    public:

        template<class T>
        LogBuffer &operator<<(T &value) {
            writeBuffer(std::to_string(value));
            return *this;
        }

        LogBuffer &operator<<(std::string &&value) {
            writeBuffer(std::move(value));
            return *this;
        }

        void writeBuffer(std::string &&logItem) {
            std::lock_guard<std::mutex> lk_m(_lk_main);
            if (_buffer_main.size() >= _flush_limit) {
                std::lock_guard<std::mutex> lk_b(_lk_back);
                swapBuffer();
            }
            _buffer_main.push(std::move(logItem));
        }

        LogBuffer(std::string logPath, std::size_t flushLimit)
                : _log_fd(FS::Open(logPath, O_WRONLY | O_CREAT | O_APPEND)), _flush_limit(flushLimit) {
            std::thread persistent(&LogBuffer::writeDisk, this);
            persistent.detach();
        }
//        static LogBuffer &getInstance(std::string logPath = "", std::size_t flushLimit = 128) {
//            static LogBuffer _instance(logPath, flushLimit);
//            return _instance;
//        }

        LogBuffer(LogBuffer const &) = delete;

        LogBuffer(LogBuffer &&) = delete;

        void operator=(LogBuffer const &) = delete;

        void operator=(LogBuffer &&) = delete;

    };

public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
//        LEVEL_NUM
    };

    static char const *LOGLEVEL[];

    static inline uint8_t GetLogLevel() {
        return _log_level;
    }

private:
    static uint8_t _log_level;
    LogBuffer _buffer;

    Logger(std::string logPath, std::size_t flushLimit, uint8_t loglevel)
            : _buffer(logPath, flushLimit) {
        _log_level = loglevel;
    }

public:

    static Logger &getInstance(std::string logPath = "", std::size_t flushLimit = 128, uint8_t loglevel = 0) {
        static Logger _instance(logPath, flushLimit, loglevel);
        return _instance;
    }

    Logger(Logger const &) = delete;

    Logger(Logger &&) = delete;

    void operator=(Logger const &) = delete;

    void operator=(Logger &&) = delete;
};

uint8_t Logger::_log_level;
char const *Logger::LOGLEVEL[]{
        "TRACE",
        "DEBUG",
        "INFO ",
        "WARN ",
        "ERROR",
        "FATAL"
};

#define LOG_TRACE if ( Logger::GetLogLevel() <= Logger::TRACE ) \
    Logger::(__FILE__, __LINE__, Logger::LOGLEVEL[Logger::TRACE], __func__).stream()

#endif //SIHTTP_LOGGER_H
