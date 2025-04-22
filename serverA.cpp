#include <iostream>
#include <fstream>
#include <unistd.h>
#include "serverA.h"

using namespace serverA;

// load credentials from file
CredentialMap serverA::loadCredentials(const std::string& filename) {
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
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_A << "." << std::endl;

    CredentialMap credentials = loadCredentials("./members.txt");
    //validation during test Phase 1
    //std::cout << "[Server A] Loaded " << credentials.size() << " users from members.txt" << std::endl;

    // stay up
    while (true) {
        sleep(1);
    }

    return 0;
}
