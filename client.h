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

    const std::string PROMPT_COMMAND      = "[Client] Please enter the command:\n> ";

    const std::string MSG_SENT_COMMAND    = "[Client] Sent command to the main server.";
    const std::string MSG_RECEIVED_REPLY  = "[Client] Received the response from the main server using TCP.";

    const std::string MSG_GOODBYE         = "[Client] Exiting.";
}

#endif
