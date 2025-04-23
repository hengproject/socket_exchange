#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "serverM.h"
#include "common_variables.h"
#include "common_sockets.h"

using namespace serverM;

// 密码加密函数
std::string serverM::encryptPassword(const std::string& password) {
    std::string encrypted = password;
    for (char& c : encrypted) {
        if (c >= 'A' && c <= 'Z')
            c = 'A' + (c - 'A' + 3) % 26;
        else if (c >= 'a' && c <= 'z')
            c = 'a' + (c - 'a' + 3) % 26;
        else if (c >= '0' && c <= '9')
            c = '0' + (c - '0' + 3) % 10;
    }
    return encrypted;
}

// 处理一个 client 的完整登录流程
void serverM::handle_single_client(int client_fd, int udp_sock) {
    // step 1: recv message from client
    auto maybe_msg = tcp_recv_string(client_fd);
    if (!maybe_msg.has_value()) {
        close(client_fd);
        return;
    }
    std::string received = maybe_msg.value();

    // step2: encrypt
    auto comma_pos = received.find(',');
    std::string username = received.substr(0, comma_pos);
    std::string password = received.substr(comma_pos + 1);

    std::cout << RECEIVED_FROM_CLIENT << username << " and password " << MSG_PASSWORD_MASK << "." << std::endl;

    std::string encrypted = encryptPassword(password);
    std::string messageToA = username + "," + encrypted;

    // step3: Send to ServerA
    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_A, messageToA);
    std::cout << SENT_TO_SERVERA << std::endl;

    // step4: Receive from ServerA
    auto authResultOpt = udp_recv_string(udp_sock);
    if (!authResultOpt.has_value()) {
        std::cerr << MSG_FAILED_FROM_SERVERA << std::endl;
        close(client_fd);
        return;
    }

    std::string authResult = authResultOpt.value();
    std::cout << RECEIVED_FROM_SERVERA << PORT_SERVER_M_UDP << std::endl;

    // step5: forward back to client
    tcp_send_string(client_fd, authResult);
    std::cout << SENT_TO_CLIENT << PORT_SERVER_M_TCP << std::endl;

    if (authResult == MSG_AUTH_GRANTED) {
        // 登录成功后进入 Phase 3 命令循环
        handle_phase3_commands(client_fd, udp_sock);
    }

    close(client_fd);
}

// Phase 3 命令分发
void serverM::handle_phase3_commands(int client_fd, int udp_sock) {
    while (true) {
        Optional<std::string> cmd = tcp_recv_string(client_fd);
        if (!cmd.has_value()) {
            std::cerr << MSG_CLIENT_DISCONNECT << std::endl;
            break;
        }

        std::string command = cmd.value();

        if (command == "exit") {
            std::cout << MSG_CLIENT_EXITED << std::endl;
            break;
        }

        // check quote command
        if (command.substr(0, 5) == "quote") {
            // forward quote command to Server Q
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, command);
            Optional<std::string> quote_result = udp_recv_string(udp_sock);

            if (!quote_result.has_value()) {
                std::cerr << MSG_FAILED_GET_QUOTE << std::endl;
                tcp_send_string(client_fd, MSG_FAILED_GET_QUOTE2);
            } else {
                tcp_send_string(client_fd, quote_result.value());
            }
        }
        // check buy command
        else if (command.substr(0, 3) == "buy") {
            std::istringstream iss(command);
            std::string cmd, stock;
            int quantity;
            iss >> cmd >> stock >> quantity;

            if (stock.empty() || quantity <= 0) {
                tcp_send_string(client_fd, MSG_BUY_INVALID);
                continue;
            }

            // Step 1: 向 serverQ 查询当前价格
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "quote " + stock);
            Optional<std::string> price_resp = udp_recv_string(udp_sock);
            if (!price_resp.has_value()) {
                tcp_send_string(client_fd, MSG_BUY_PRICE_FAIL);
                continue;
            }

            std::istringstream price_iss(price_resp.value());
            std::string stock_reply;
            double price;
            price_iss >> stock_reply >> price;

            // Step 2: 更新 serverP
            std::ostringstream msg_to_P;
            msg_to_P << "buy " << stock << " " << quantity << " " << price;
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_P, msg_to_P.str());

            // Step 3: 通知 serverQ 推进价格
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "advance " + stock);

            // Step 4: 回复 client
            tcp_send_string(client_fd, MSG_BUY_SUCCESS);
        }
        else {
            tcp_send_string(client_fd, MSG_UNKNOWN_COMMAND);
        }
    }
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_M_UDP << "." << std::endl;
    std::cout << TCP_LISTEN_MESSAGE << PORT_SERVER_M_TCP << "." << std::endl;

    // set up tcp & udp server
    int tcp_sock = create_tcp_server_socket(LOCALHOST, PORT_SERVER_M_TCP);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_M_UDP);

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(tcp_sock, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        serverM::handle_single_client(client_fd, udp_sock);
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
