#ifndef SERVERQ_H
#define SERVERQ_H

#include "common_variables.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace serverQ {
    const std::string BOOTUP_MESSAGE       = "[Server Q] Booting up using UDP on port ";
    const std::string QUOTE_FILE           = "quotes.txt";

    // Command keywords
    const std::string CMD_QUOTE            = "quote";

    // Response strings
    const std::string RESP_UNKNOWN_COMMAND = "[Server Q] Unknown command.\n";
    const std::string RESP_UNKNOWN_STOCK   = "[Server Q] Unknown stock: ";
    const std::string CMD_QUOTE    = "quote";
    const std::string CMD_ADVANCE  = "advance";

    const std::string RESP_ADVANCE_OK = "[Server Q] Price advanced for ";
    const std::string RESP_ADVANCE_FAIL = "[Server Q] Failed to advance: unknown stock ";

    struct QuoteInfo {
        std::vector<double> prices;
        int current_index = 0;

        double current_price() const {
            return prices[current_index];
        }
    };

    using QuoteMap = std::unordered_map<std::string, QuoteInfo>;

    QuoteMap loadQuotes(const std::string& filename);
}

#endif
