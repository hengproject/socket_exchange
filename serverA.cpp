#include <iostream>
#include <string>
#include <unistd.h>
#include "serverA.h"
#include "common_sockets.h"

// load credentials from file
serverA::CredentialMap serverA::loadCredentials(const std::string& filename) {
    CredentialMap creds;
    std::ifstream infile(filename);
    std::string username, password;

    if (!infile) {
        std::cerr << "[Server A] Failed to open " << filename << std::endl;
        return creds;
    }

    while (infile >> username >> password) {
        creds[username] = password;
    }

    return creds;
}

int main() {
    std::cout << serverA::BOOTUP_MESSAGE << PORT_SERVER_A << "." << std::endl;

    // Load credential map from file
    serverA::CredentialMap credentials = serverA::loadCredentials(serverA::REQUEST_FILE);

    // Create UDP socket
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_A);

    while (true) {
        Optional<std::string> request = udp_recv_string(udp_sock);
        if (!request.has_value()) {
            std::cerr << "[Server A] Failed to receive request." << std::endl;
            continue;
        }

        std::string msg = request.value();
        std::string username, encrypted_pw;

        size_t comma_pos = msg.find(',');
        if (comma_pos != std::string::npos) {
            username = msg.substr(0, comma_pos);
            encrypted_pw = msg.substr(comma_pos + 1);
        } else {
            std::cerr << "[Server A] Invalid format received: " << msg << std::endl;
            continue;
        }

        std::cout << "[Server A] Received username " << username
                  << " and password " << MSG_PASSWORD_MASK  << "." << std::endl;

        // Make username case-insensitive
        std::string lowered_username = username;
        for (auto& c : lowered_username) c = tolower(c);

        std::string stored_pw;
        bool found = false;

        for (const auto& pair : credentials) {
            std::string lowered_stored_user = pair.first;
            for (auto& c : lowered_stored_user) c = tolower(c);

            if (lowered_stored_user == lowered_username) {
                stored_pw = pair.second;
                found = true;
                break;
            }
        }

        if (!found || stored_pw != encrypted_pw) {
            std::cout << "[Server A] The username " << username
                      << " or password " << MSG_PASSWORD_MASK  << " is incorrect." << std::endl;
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, MSG_AUTH_DENIED);
        } else {
            std::cout << "[Server A] Member " << username << " has been authenticated." << std::endl;
            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, MSG_AUTH_GRANTED);
        }
    }

    close(udp_sock);
    return 0;
}
