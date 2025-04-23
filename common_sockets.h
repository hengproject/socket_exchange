#ifndef COMMON_SOCKETS_H
#define COMMON_SOCKETS_H

#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "common_variables.h"

// GPT generated code, see README.md
// C++11 does not support Optional
template<typename T>
class Optional {
private:
    bool valid;
    T data;
public:
    Optional() : valid(false) {}
    Optional(const T& value) : valid(true), data(value) {}

    bool has_value() const {
        return valid;
    }

    T value() const {
        if (!valid) {
            throw std::runtime_error("Accessing empty Optional");
        }
        return data;
    }

    T value_or(const T& default_val) const {
        return valid ? data : default_val;
    }
};

//
// ==============================
// ==== TCP FUNCTIONS ==========
// ==============================
//

// TCP Server: 创建监听 socket 并绑定本地端口
inline int create_tcp_server_socket(const std::string& ip, int port) {
    // 创建 socket，AF_INET -> IPv4，SOCK_STREAM -> TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // 绑定地址
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    // 开始监听连接请求
    if (listen(sockfd, MAX_PENDING_CLIENTS) < 0) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

// TCP Client: 创建 socket 并连接 serverM
inline int create_tcp_client_socket(const std::string& ip, int port) {
    // create a socket, AF_INET -> IPV4 SOCKET_STREAM -> TCP
    // when using SOCK_STREAM or SOCK_DGRAM, you can just set the protocol to 0
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // AF_INET -> IPV4, htons: host to network short
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    serv_addr.sin_port = htons(port);

    // connect with serverM, ret = -1 --> error
    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

// TCP 接收字符串数据（支持 null 终止），封装为 Optional
inline Optional<std::string> tcp_recv_string(int sockfd, size_t max_len = 256) {
    char* buffer = new char[max_len];
    std::memset(buffer, 0, max_len);

    int bytes_received = recv(sockfd, buffer, max_len - 1, 0);
    if (bytes_received <= 0) {
        delete[] buffer;
        return Optional<std::string>(); // empty
    }

    buffer[bytes_received] = '\0';
    std::string result(buffer);
    delete[] buffer;
    return Optional<std::string>(result);
}

// TCP 发送字符串数据
inline bool tcp_send_string(int sockfd, const std::string& msg) {
    int sent = send(sockfd, msg.c_str(), msg.size(), 0);
    if (sent < 0) return false;
    return (size_t)sent == msg.size();
}

//
// ==============================
// ==== UDP FUNCTIONS ==========
// ==============================
//

// UDP Server: 创建 socket 并绑定端口
inline int create_udp_server_socket(const std::string& ip, int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

// UDP 发送字符串数据到指定地址
inline void udp_send_string(int sockfd, const std::string& ip, int port, const std::string& message) {
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(ip.c_str());
    dest.sin_port = htons(port);

    sendto(sockfd, message.c_str(), message.size(), 0, (sockaddr*)&dest, sizeof(dest));
}

// UDP 接收字符串数据（封装为 Optional）
inline Optional<std::string> udp_recv_string(int sockfd, size_t max_len = 256) {
    char* buffer = new char[max_len];
    std::memset(buffer, 0, max_len);

    sockaddr_in src_addr{};
    socklen_t addr_len = sizeof(src_addr);
    int bytes_received = recvfrom(sockfd, buffer, max_len - 1, 0, (sockaddr*)&src_addr, &addr_len);

    if (bytes_received <= 0) {
        delete[] buffer;
        return Optional<std::string>();
    }

    buffer[bytes_received] = '\0';
    std::string result(buffer);
    delete[] buffer;
    return Optional<std::string>(result);
}

#endif // COMMON_SOCKETS_H
