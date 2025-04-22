#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "serverM.h"
#include "common_variables.h"
#include "common_sockets.h"

using namespace serverM;

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

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_M_UDP << "." << std::endl;
    std::cout << TCP_LISTEN_MESSAGE << PORT_SERVER_M_TCP << "." << std::endl;
    //set up tcp& udp server
    int tcp_sock = create_tcp_server_socket(LOCALHOST, PORT_SERVER_M_TCP);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_M_UDP);

    while (true) {
    // step 1:recv message form client
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(tcp_sock, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        //std::cout << "Client connected" << std::endl;

        auto maybe_msg = tcp_recv_string(client_fd);
        if (!maybe_msg.has_value()) {
            close(client_fd);
            continue;
        }
        std::string received = maybe_msg.value();
        // step2: encrypt
        auto comma_pos = received.find(',');
        std::string username = received.substr(0, comma_pos);
        std::string password = received.substr(comma_pos + 1);

        std::cout << RECEIVED_FROM_CLIENT << username << " and password "<<MSG_PASSWORD_MASK <<"." << std::endl;

        std::string encrypted = encryptPassword(password);
        std::string messageToA = username + "," + encrypted;

        // step3: Send to ServerA
        udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_A, messageToA);
        std::cout << SENT_TO_SERVERA << std::endl;

        // step4: Receive from ServerA
        auto authResultOpt = udp_recv_string(udp_sock);
        if (!authResultOpt.has_value()) {
            std::cerr << "[Server M] Failed to receive response from Server A." << std::endl;
            close(client_fd);
            continue;
        }

        std::string authResult = authResultOpt.value();
        std::cout << RECEIVED_FROM_SERVERA << PORT_SERVER_M_UDP << std::endl;

        // step5: forward back to client
        tcp_send_string(client_fd, authResult);
        std::cout << SENT_TO_CLIENT << PORT_SERVER_M_TCP << std::endl;

        close(client_fd);
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
