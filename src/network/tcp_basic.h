//
// Created by Sinow on 2022/3/23.
//

#ifndef SINO_TCP_BASIC_H
#define SINO_TCP_BASIC_H

// socket(), connect(), bind()
// accept4()
// AF_INET, SOCK_STREAM,
// SOCK_NONBLOCK (set O_NONBLOCK on invoking socket())
// SOCK_CLOEXEC (set O_CLOEXEC on invoking socket())
#include <sys/socket.h>
// IPv4 Address Utilities
#include "network/internet.h"
// File Descriptor
#include "system/file_descriptor.h"

#include <vector>

// std::numeric_limits
#include <limits>

namespace TCP {

    using socket_type_t = int;
    using socket_protocol_t = int;

    socket_type_t constexpr SOCKET_TYPE_TCP = AF_INET;
    socket_protocol_t constexpr SOCKET_PROTOCOL_TCP = SOCK_STREAM;

    // Since Linux 5.4, the default in this file is 4096;
    // in earlier kernels, the default value is 128.
    // In kernels before 2.4.25, this limit was a hard coded value, 
	// SOMAXCONN, with the value 128.
    int constexpr PENDING_QUEUE_LENGTH = 128;

    class TCP_Base {

    private:
		// int _fd{};
        FD::FileDescriptorPtr _fd{nullptr};
    public:
        // 若提供 ipv4_address 与 port，则 bind 到当前的 _fd
        TCP_Base(std::string const &ipv4_address = "", uint16_t port = 0,
				bool nonblock=false, bool cloexec=true);

        // 若提供 ipv4_address 与 port，则 bind 到当前的 _fd
        // char * version
        TCP_Base(char const *ipv4_address = nullptr, uint16_t port = 0,
				bool nonblock=false, bool cloexec=true);

	public:
		// construct with a FileDescriptorPtr
		// it shoud be a valid socket fd
		// don not bind again
		TCP_Base(FD::FileDescriptorPtr fd);

    public:
        // (重新建立 socket)绑定 ipv4_address 与 port 当前的 _fd
        // if ipv4_address == "", then kernel will choose an address
        // if port == 0, then kernel will choose a random port
        void Bind(std::string const &ipv4_address = "", uint16_t port = 0);

        // (重新建立 socket)绑定 ipv4_address 与 port 当前的 _fd
        // if ipv4_address == "", then kernel will choose an address
        // if used for listen, it will listen on any available address on
		// this machine
        // if port == 0, then kernel will choose a random port
        // char * version
        void Bind(char const *ipv4_address = nullptr, uint16_t port = 0);

    public:
		// connect 失败之后应该关闭该socket并重新创建后再重试
        // 与给出的 ipv4_address:port 进行建立连接
        void Connect(std::string const &ipv4_address, uint16_t port);

        // 与给出的 ipv4_address:port 进行建立连接
        // char * version
        void Connect(char const *ipv4_address, uint16_t port);

    public:
        // 在 ipv4_address:port 上 listen, 若未指定，则与上次修改一致
        void Listen(int pending_length = PENDING_QUEUE_LENGTH, std::string const &ipv4_address = "", uint16_t port = 0);

        // 在 ipv4_address:port 上 listen, 若未指定，则与上次修改一致
        // char * version
        void Listen(int pending_length = PENDING_QUEUE_LENGTH, char const *ipv4_address = "", uint16_t port = 0);

    public:
		// reset FileDescriptorPtr _fd, actually invoke shutdown()
		void Close();

	public:
		// invoke accept4 to use flags
		TCP_Base Accept(bool nonblock=false, bool cloexec=true);

	public:
		// invoke getsockaddr
		std::tuple<std::string, uint16_t> GetSockAddress();
		// invoke getpeeraddr
		std::tuple<std::string, uint16_t> GetPeerAddress();

	public:
		// return sent data size
		template <typename T>
		ssize_t Send(std::vector<T> const& data) {
			static_assert(std::numeric_limits<T>::is_integer, "T can only be bool, ints, chars");

		}
		// return sent data size
		ssize_t Send(std::string const& data);

	public:
		std::vector<char> Recv(ssize_t size);
		std::vector<char> Peek(ssize_t size);

	public:
		// 套接字选项设置

    };
}

#endif //SINO_TCP_BASIC_H
