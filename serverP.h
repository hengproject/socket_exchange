#ifndef SERVERP_H
#define SERVERP_H

#include "common_variables.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace serverP {
    const std::string BOOTUP_MESSAGE = "[Server P] Booting up using UDP on port ";
    const std::string PORTFOLIO_FILE = "portfolios.txt";

    struct StockHolding {
        std::string stock;
        int quantity = 0;
        double avg_price = 0.0;
    };

    using Portfolio = std::unordered_map<std::string, StockHolding>;  // stock → 持仓
    using UserMap = std::unordered_map<std::string, Portfolio>;       // user → portfolio

    UserMap loadPortfolios(const std::string& filename);
}

#endif
