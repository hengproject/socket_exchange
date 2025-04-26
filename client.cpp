#include <iostream>
#include <tuple>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "common_variables.h"
#include "common_sockets.h"
#include "client.h"

void enter_command_loop(int sockfd,const std::string& username);
bool confirm_YN(const std::string& prompt);

std::tuple<std::string, std::string, std::string> parseMessage(const std::string& message) {
    size_t firstComma = message.find(',');
    size_t secondComma = message.find(',', firstComma + 1);

    std::string type, status, content;
    if (firstComma != std::string::npos) {
        type = message.substr(0, firstComma);
    }
    if (secondComma != std::string::npos) {
        status = message.substr(firstComma + 1, secondComma - firstComma - 1);
    }
    if (secondComma != std::string::npos) {
        content = message.substr(secondComma + 1);
    }

    return std::make_tuple(type, status, content);
}

void QuoteResponseHandler(const std::string& status, const std::string& content) {
    std::cout << "[Client] Received the response from the main server using TCP over port <client port number>. " << std::endl;
    if (status == "OK") {
        std::cout << content << std::endl;
    } else if (status == "ERROR") {
        std::cout << content <<" does not exist. Please try again. "<< std::endl;
    }
}

void PositionResponseHandler(const std::string& status, const std::string& content) {

    return;
}

void BuyResponseHandler(const std::string& status, const std::string& content) {
    return;
}

void SellResponseHandler(const std::string& status, const std::string& content) {
	return;
}

// 这是你要的 router
void HandleServerReply(const std::string& type, const std::string& status, const std::string& content) {
    if (type == "QUOTE") {
        QuoteResponseHandler(status, content);
    } else if (type == "POSITION") {
        PositionResponseHandler(status, content);
    } else if (type == "BUY") {
        BuyResponseHandler(status, content);
    } else if (type == "SELL") {
        SellResponseHandler(status, content);
    } else {
        std::cerr << "Unknown response type: " << type << std::endl;
    }
}

int main() {
    std::cout << client::BOOTUP_MESSAGE << std::endl;

    // create tcp client socket
    int sockfd = create_tcp_client_socket(LOCALHOST, PORT_SERVER_M_TCP);

    std::cout << client::LOGIN_PROMPT << std::endl;

    while (true) {
        // for test
       //std::string username, password;
       //std::cout << client::PROMPT_USERNAME;
       //std::getline(std::cin, username);
       //std::cout << client::PROMPT_PASSWORD;
       //std::getline(std::cin, password);

       //if (username.empty() || password.empty()) {
       //    std::cerr << "[Client] Username and password must not be empty." << std::endl;
       //    continue;
       //}
       //if (username.length() > 51 || password.length() > 51) {
       //    std::cerr << "[Client] Username/password too long." << std::endl;
       //    continue;
       //}

       //std::string message = username + "," + password;
        std::string username="*";
        std::string message = "*,*";
        tcp_send_string(sockfd, message);

        auto reply = tcp_recv_string(sockfd);
        if (!reply.has_value()) {
            std::cerr << client::CONNECTION_ERROR << std::endl;
            break;
        }

        std::string response = reply.value();
        if (response == MSG_AUTH_GRANTED) {
            std::cout << client::PROMPT_CORRECT_CREDENTIAL << std::endl;
            enter_command_loop(sockfd,username);
            break;
        } else {
            std::cout << client::PROMPT_WRONG_CREDENTIAL << std::endl;
        }
    }

    close(sockfd);
    return 0;
}



void enter_command_loop(int sockfd, const std::string& username) {
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

        std::string server_reply = reply.value();
        std::cout << client::MSG_RECEIVED_REPLY_WITHOUT_PORT << PORT_SERVER_M_TCP << "." << std::endl;
		std::cout << server_reply << std::endl;

        std::cout << "----- Start a new request -----" << std::endl;
    }
}
