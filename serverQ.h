#ifndef SERVERQ_H
#define SERVERQ_H

#include "common_variables.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace serverQ {
    const std::string BOOTUP_MESSAGE = "[Server Q] Booting up using UDP on port ";

    using PriceList = std::vector<double>;

    struct StockQuote {
        PriceList prices;
        int currentIndex = 0;

        double currentPrice() const {
            return prices[currentIndex];
        }

        void advance() {
            currentIndex = (currentIndex + 1) % prices.size();
        }
    };

    using QuoteMap = std::unordered_map<std::string, StockQuote>;

    QuoteMap loadQuotes(const std::string& filename);
}

#endif
