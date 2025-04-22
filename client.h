#ifndef CLIENT_H
#define CLIENT_H

#include <string>

namespace client {
    const std::string BOOTUP_MESSAGE      = "[Client] Booting up.";
    const std::string LOGIN_PROMPT        = "[Client] Logging in.";
    const std::string ACCESS_GRANTED      = "[Client] You have been granted access.";
    const std::string ACCESS_DENIED       = "[Client] The credentials are incorrect. Please try again.";
    const std::string PROMPT_USERNAME     = "Please enter the username: ";
    const std::string PROMPT_PASSWORD     = "Please enter the password: ";
    const std::string CONNECTION_ERROR    = "[Client] Connection closed or failed.";

}

#endif
