#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "serverM.h"

int main() {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        perror("socket creation failed");
        return 1;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servAddr.sin_port = htons(PORT_SERVER_M_UDP);

    if (bind(udpSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    std::cout << serverM::BOOTUP_MESSAGE << PORT_SERVER_M_UDP << "." << std::endl;

    while (true) {
        sleep(1);
    }

    close(udpSocket);
    return 0;
}
