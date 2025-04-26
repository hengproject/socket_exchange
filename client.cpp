#include <iostream>
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

// ------------ NEW 确认函数 ------------ //
bool confirm_YN(const std::string& prompt) {
    std::string input;
    std::cout << prompt << " (Y/N): ";
    std::getline(std::cin, input);
    return input == "Y" || input == "y";
}

void handle_OK_reply(const std::string& reply, const std::string& username) {
    std::istringstream iss(reply);
    std::string ok_word;
    std::string next_word;
    iss >> ok_word >> next_word;

    if (next_word == "sell") {
        // 处理卖出成功
        std::string stock;
        int remaining_quantity;
        std::string at_word;
        double avg_price;
        iss >> stock >> remaining_quantity >> at_word >> avg_price;

        std::cout << "[Client] " << username << " successfully sold shares of " << stock << "." << std::endl;
    } else {
        // 处理position查询
        std::string line;
        std::getline(iss, line); // 直接跳过第一行（next_word可能不完整，干脆新起一行）
        while (std::getline(iss, line)) {
            std::cout << line << std::endl;
        }
    }
}



void handle_ERR_reply(const std::string& reply, const std::string& username) {
    if (reply.find("stock name does not exist") != std::string::npos) {
        std::cout << "[Client] Error: stock name does not exist. Please check again." << std::endl;
    } else if (reply.find("not enough shares") != std::string::npos) {
        std::cout << "[Client] Error: " << username << " does not have enough shares to sell. Please try again." << std::endl;
    } else if (reply.find("invalid command") != std::string::npos) {
        std::cout << "[Client] Error: stock name/shares are required. Please specify a stock name to sell." << std::endl;
    } else if (reply.find("denied by user") != std::string::npos) {
        std::cout << "[Client] Sell denied." << std::endl;
    } else {
        std::cout << "[Client] Error: " << reply << std::endl;
    }
}

void handle_CONFIRM_reply(int sockfd, const std::string& confirm_msg, const std::string& username) {
    // 解析 CONFIRM 类型的消息，比如：
    // "CONFIRM sell S1 10 at 103.0"
    // 将来buy也可以复用

    std::istringstream iss(confirm_msg);
    std::string confirm_word, action, stock;
    int quantity;
    std::string at_word;
    double price;
    iss >> confirm_word >> action >> stock >> quantity >> at_word >> price;

    if (action == "sell") {
        std::cout << "[Client] " << stock << "’s current price is " << price << ". Proceed to sell? (Y/N)" << std::endl;
    } else if (action == "buy") {
        std::cout << "[Client] " << stock << "’s current price is " << price << ". Proceed to buy? (Y/N)" << std::endl;
    } else {
        std::cout << "[Client] Unknown confirm action: " << action << std::endl;
    }

    std::string choice;
    std::getline(std::cin, choice);
    if (choice == "Y" || choice == "y") {
        tcp_send_string(sockfd, "Y");
    } else {
        tcp_send_string(sockfd, "N");
    }

    // 等待服务器返回最终结果
    Optional<std::string> final_reply = tcp_recv_string(sockfd);
    if (!final_reply.has_value()) {
        std::cerr << client::CONNECTION_ERROR << std::endl;
        return;
    }

    std::string result = final_reply.value();
    if (result.rfind("OK", 0) == 0) {
        handle_OK_reply(result, username);
    } else {
        handle_ERR_reply(result, username);
    }
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
        // 分类处理 server 回复
        if (server_reply.rfind("OK", 0) == 0) {
            handle_OK_reply(server_reply, username);

        } else if (server_reply.rfind("ERR", 0) == 0) {
            handle_ERR_reply(server_reply, username);

        } else if (server_reply.rfind("CONFIRM", 0) == 0) {
            handle_CONFIRM_reply(sockfd, server_reply, username);

        } else {
            std::cout << "[Client] Unknown server response: " << server_reply << std::endl;
        }

        std::cout << "----- Start a new request -----" << std::endl;
    }
}
