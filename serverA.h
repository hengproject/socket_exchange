#ifndef SERVERA_H
#define SERVERA_H

#include "common_variables.h"
#include <string>
#include <unordered_map>

namespace serverA {
    const std::string BOOTUP_MESSAGE = "[Server A] Booting up using UDP on port ";

    using CredentialMap = std::unordered_map<std::string, std::string>;

    CredentialMap loadCredentials(const std::string& filename);
}

#endif
