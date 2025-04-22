#include <iostream>
#include <unistd.h>
#include "client.h"

int main() {
    std::cout << client::BOOTUP_MESSAGE << std::endl;

    // Phase 1: 保持运行
    while (true) {
        sleep(1);
    }

    return 0;
}
