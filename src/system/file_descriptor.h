//
// Created by Sinow on 2022/3/23.
//

#ifndef SINO_FILE_DESCRIPTOR_H
#define SINO_FILE_DESCRIPTOR_H

// close()
#include <unistd.h>
// std::unique_ptr<FileDescriptor>, std::make_unique<FileDecriptor>()
#include <memory>

// stat() fstat() lstat() fstatat()
// S_ISREG()
#include <sys/stat.h>
// shutdown()
#include <sys/socket.h>

#include "general/inc_exception.h"

// DON'T ACCESS THIS NAMESPACE DIRECTLY
// YOU CAN'T INSURE FILE DESCRIPTOR IS ALIVE
namespace FD {
    class FileDescriptor final {
    private:

		class Type {
			public:
			enum : uint8_t{
				REGULAR, // 常规文件
				DIRECTORY, // 目录
				BLOCK, // 块文件
				CHARACTOR, // 字符文件
				FIFO, // 管道或FIFO
				SYMLINK, // 符号链接
				SOCKET, // 套接字
				
				// IPC TYPE
				MSGQUEUE, // 消息队列
				SEMOPHORE, // 信号量
				SHAREMEM, // 共享存储对象
			};
		};
        int _fd;
		// file type
		uint8_t _type;
		// for destructor judge fd infomation
		struct stat _stat{};
	public:
		int Get() const { return _fd; }

        // put an opened fd
        FileDescriptor(int const fd) : _fd{fd} {
			if (fstat(_fd, &_stat)==-1) {
				throw std::runtime_error(strerror(errno));
			}

			if(S_ISREG(_stat.st_mode)) {
				_type = Type::REGULAR;
			} else if (S_ISDIR(_stat.st_mode)) {
				_type = Type::DIRECTORY;
			} else if (S_ISBLK(_stat.st_mode)) {
				_type = Type::BLOCK;
			} else if (S_ISCHR(_stat.st_mode)) {
				_type = Type::CHARACTOR;
			} else if (S_ISFIFO(_stat.st_mode)) {
				_type = Type::FIFO;
			} else if (S_ISLNK(_stat.st_mode)) {
				_type = Type::SYMLINK;
			} else if (S_ISSOCK(_stat.st_mode)) {
				_type = Type::SOCKET;
			} else if (S_TYPEISMQ(_stat)) {
				_type = Type::MSGQUEUE;
			} else if (S_TYPEISSEM(_stat)) {
				_type = Type::SEMOPHORE;
			} else if (S_TYPEISSHM(_stat)) {
				_type = Type::SHAREMEM;
			}
		}

        // friend std::make_unique to construct the private FileDescriptor constructor
        friend std::unique_ptr<FileDescriptor> std::make_unique<FileDescriptor>(int const &);

        // put an opened fd
//        friend std::unique_ptr<FileDescriptor> makeFileDescriptor(int const fd);

    public:

        ~FileDescriptor() noexcept {
			switch (_type) {
				case Type::REGULAR:
					break;
				case Type::DIRECTORY:
					break;
				case Type::BLOCK:
					break;
				case Type::CHARACTOR:
					break;
				case Type::FIFO:
					break;
				case Type::SYMLINK:
					break;
				case Type::SOCKET:
					switch(shutdown(_fd, SHUT_WR)) {
						case EBADF: // invalid file descriptor
						case EINVAL: // never happen here
						case ENOTCONN: // socket not connected
						case ENOTSOCK: // never happen here
							break;
					}
					break;
				case Type::MSGQUEUE:
					break;
				case Type::SEMOPHORE:
					break;
				case Type::SHAREMEM:
					break;
			}
        }

        // below feature can be reached by being wrapped by std::unique_ptr
//        // is_not_copy_constructible
//        FileDescriptor(FileDescriptor const &) = delete;
//
//        // is_not_copy_assignable
//        void operator=(FileDescriptor const &) = delete;
//
//        // is_move_constructible
//        FileDescriptor(FileDescriptor &&instance) noexcept = default;
//
//        // is_not_move_assignable
//        FileDescriptor &operator=(FileDescriptor &&instance) noexcept = default;
    };

    using FileDescriptorPtr = std::unique_ptr<FileDescriptor>;
    FileDescriptorPtr makeFileDecriptor(int const fd) {
        return std::make_unique<FileDescriptor>(fd);
    }

}

#endif //SINO_FILE_DESCRIPTOR_H
