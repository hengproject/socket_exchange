#ifndef SERVERQ_H
#define SERVERQ_H

#include "common_variables.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace serverQ {
    const std::string BOOTUP_MESSAGE       = "[Server Q] Booting up using UDP on port ";
    const std::string QUOTE_FILE           = "quotes.txt";

    // Response strings
    const std::string RESP_UNKNOWN_COMMAND = "[Server Q] Unknown command.\n";
    const std::string RESP_STOCK_404   = " does not exist. Please try again. ";
    const std::string CMD_QUOTE    = "quote";
    const std::string CMD_ADVANCE  = "advance";

    const std::string RESP_ADVANCE_OK = "[Server Q] Price advanced for ";
    const std::string RESP_ADVANCE_FAIL = "[Server Q] Failed to advance: unknown stock ";
    const std::string RECIEVED_GENERAL_QOUTE = "[Server Q] Received a quote request from the main server.";
    const std::string RECIEVED_SPECIFIC_QOUTE = "[Server Q] Received a quote request from the main server.";
    const std::string GENERAL_QUOTE_RET = "[Server Q] Returned all stock quotes.";
    const std::string SPECIFIC_QOUTE_RET = "[Server Q] Returned the stock quote of ";

    struct QuoteInfo {
        std::vector<double> prices;
        int current_index = 0;

        double current_price() const {
            return prices[current_index];
        }
        void advance() {
            current_index = (current_index + 1) % prices.size();
        }
    };

    using QuoteMap = std::unordered_map<std::string, QuoteInfo>;

    QuoteMap loadQuotes(const std::string& filename);
}

#endif
