#ifndef SERVERM_H
#define SERVERM_H

#include "common_variables.h"
#include <string>

namespace serverM {
    const std::string BOOTUP_MESSAGE        = "[Server M] Booting up using UDP on port ";
    const std::string TCP_LISTEN_MESSAGE    = "[Server M] Listening for clients on TCP port ";

    const std::string RECEIVED_FROM_CLIENT  = "[Server M] Received username ";
    const std::string SENT_TO_SERVERA       = "[Server M] Sent the authentication request to Server A";
    const std::string RECEIVED_FROM_SERVERA = "[Server M] Received the response from Server A using UDP over ";
    const std::string SENT_TO_CLIENT        = "[Server M] Sent the response from server A to the client using TCP over ";

    std::string encryptPassword(const std::string& password);
}

#endif
