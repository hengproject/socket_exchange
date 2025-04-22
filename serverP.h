#ifndef SERVERP_H
#define SERVERP_H

#include "common_variables.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace serverP {
    const std::string BOOTUP_MESSAGE = "[Server P] Booting up using UDP on port ";

    struct StockHolding {
        std::string stockName;
        int shares;
        double avgBuyPrice;
    };

    using Portfolio = std::vector<StockHolding>;
    using PortfolioMap = std::unordered_map<std::string, Portfolio>;

    PortfolioMap loadPortfolios(const std::string& filename);
}

#endif
