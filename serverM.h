#ifndef SERVERM_H
#define SERVERM_H

#include "common_variables.h"
#include <string>

namespace serverM {
    const std::string BOOTUP_MESSAGE          = "[Server M] Booting up using UDP on port ";
    const std::string TCP_LISTEN_MESSAGE      = "[Server M] Listening for clients on TCP port ";

    const std::string RECEIVED_FROM_CLIENT    = "[Server M] Received username ";
    const std::string SENT_TO_SERVERA         = "[Server M] Sent the authentication request to Server A";
    const std::string RECEIVED_FROM_SERVERA   = "[Server M] Received the response from Server A using UDP over ";
    const std::string SENT_TO_CLIENT          = "[Server M] Sent the response from Server A to the client using TCP over ";

    const std::string MSG_CLIENT_DISCONNECT   = "[Server M] Client disconnected.";
    const std::string MSG_CLIENT_EXITED       = "[Server M] Client exited session.";
    const std::string MSG_UNKNOWN_COMMAND     = "[Server M] Unknown command.";
    const std::string MSG_FAILED_GET_QUOTE    = "[Server M] Failed to get quote from Server Q.";
    const std::string MSG_FAILED_GET_QUOTE2   = "[Server M] Failed to retrieve quote.";
    const std::string MSG_FAILED_FROM_SERVERA = "[Server M] Failed to receive response from Server A.";

    const std::string MSG_BUY_SUCCESS = "[Server M] Purchase completed.";
    const std::string MSG_BUY_INVALID = "[Server M] Invalid buy command.";
    const std::string MSG_BUY_PRICE_FAIL = "[Server M] Failed to get price from Server Q.";


    std::string encryptPassword(const std::string& password);
    void handle_single_client(int client_fd, int udp_sock);
    void handle_phase3_commands(int client_fd, int udp_sock);
}

#endif
