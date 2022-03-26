//
// Created by Sinow on 2022/3/23.
// 对 IPv4 地址进行封装，提供IPv4地址转化，端口转化， sockaddr_in 设置、修改、读取 等功能

#ifndef SINO_INTERNET_H
#define SINO_INTERNET_H

// bzero();
#include <cstring>
// inet_aton(), inet_ntoa()
#include <arpa/inet.h>
// struct sockaddr_in; htons(), htonl(), ntohs(), ntohl();
#include <netinet/in.h>
// struct sockaddr;
#include <sys/socket.h>

#include <string>


namespace INET {

    using ipv4_sockaddr_t = struct sockaddr_in;
    using general_sockaddr_t = struct sockaddr;
    using bits_ipv4_t = struct in_addr;

    using port_t = uint16_t;

    class Inet final {
    private:
        ipv4_sockaddr_t _sockaddr_in{};

    public:
        // 返回值指向 Inet 非静态成员变量, 注意及时使用或拷贝
        // std::string version
        general_sockaddr_t &get_general_sockaddr(std::string const &ipv4_address);

        // 返回值指向 Inet 非静态成员变量, 注意及时使用或拷贝
        // char * version
        general_sockaddr_t &get_general_sockaddr(char const *ipv4_address);

    public:
        // host-endian to network-endian
        port_t port_to_network(uint16_t const port);

        // network-endian to host-endian
        uint16_t port_to_host(port_t const port);

    public:
        // IPv4 地址网络字节序二进制 转 ASCII码字符串
        std::string bits_to_ascii(bits_ipv4_t ipv4_address);

        // IPv4 地址网络字节序二进制 转 ASCII码字符串
        // char * version, write directly to result
        void bits_to_ascii(bits_ipv4_t ipv4_addr, char *result);

        // ASCII码字符串 转 IPv4 地址网络字节序二进制
        bits_ipv4_t ascii_to_bits(std::string ipv4_addr);
    };

}


#endif //SINO_INTERNET_H
