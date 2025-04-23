#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "common_variables.h"
#include "common_sockets.h"
#include "client.h"

void enter_command_loop(int sockfd);
int main() {
    std::cout << client::BOOTUP_MESSAGE << std::endl;

    // create tcp client socket
    int sockfd = create_tcp_client_socket(LOCALHOST, PORT_SERVER_M_TCP);

    std::cout << client::LOGIN_PROMPT << std::endl;

    // Phase 2: connect with tcp server (attempt)
    while (true) {
        std::string username, password;
        std::cout << client::PROMPT_USERNAME;
        std::getline(std::cin, username);
        std::cout << client::PROMPT_PASSWORD;
        std::getline(std::cin, password);

        std::string message = username + "," + password;

        // ssize_t send(int s, const void *buf, size_t len, int flags);
        // send for tcp, sendto for udp
        tcp_send_string(sockfd, message);

        auto reply = tcp_recv_string(sockfd);
        if (!reply.has_value()) {
            std::cerr << client::CONNECTION_ERROR << std::endl;
            break;
        }

        std::string response = reply.value();

        if (response == MSG_AUTH_GRANTED) {
            std::cout << MSG_AUTH_GRANTED << std::endl;
			// Phase 3
			enter_command_loop(sockfd);
			// to end the loop
            break;
        } else {
            std::cout << MSG_AUTH_DENIED << std::endl;
        }
    }

    return 0;
}
void enter_command_loop(int sockfd) {
    while (true) {
        std::cout << client::PROMPT_COMMAND;
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << client::MSG_GOODBYE << std::endl;
            break;
        }

        tcp_send_string(sockfd, command);
        std::cout << client::MSG_SENT_COMMAND << std::endl;

        Optional<std::string> reply = tcp_recv_string(sockfd);
        if (!reply.has_value()) {
            std::cerr << client::CONNECTION_ERROR << std::endl;
            break;
        }

        std::cout << client::MSG_RECEIVED_REPLY << std::endl;
        std::cout << reply.value() << std::endl;
        std::cout << "----- Start a new request -----" << std::endl;
    }
}
//James,SODids392