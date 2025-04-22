#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "serverP.h"

using namespace serverP;

PortfolioMap serverP::loadPortfolios(const std::string& filename) {
    PortfolioMap portfolios;
    std::ifstream infile(filename);
    std::string line;
    std::string currentUser;

    while (std::getline(infile, line)) {
        // 去除左右空格
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;  // 跳过空行

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() == 1) {
            // 是用户名行
            currentUser = tokens[0];
            portfolios[currentUser] = Portfolio();
        } else if (tokens.size() == 3) {
            // 是股票记录行
            if (currentUser.empty()) {
                std::cerr << "[Server P] Error: Stock line before username." << std::endl;
                continue;
            }
            StockHolding holding;
            holding.stockName = tokens[0];
            holding.shares = std::stoi(tokens[1]);
            holding.avgBuyPrice = std::stod(tokens[2]);
            portfolios[currentUser].push_back(holding);
        } else {
            std::cerr << "[Server P] Warning: Invalid line: " << line << std::endl;
        }
    }

    return portfolios;
}


int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_P << "." << std::endl;

    PortfolioMap data = loadPortfolios("portfolios.txt");
    // Phase 1 debugging
    //std::cout << "[Server P] Loaded portfolios for " << data.size() << " users from portfolios.txt" << std::endl;
    //std::cout <<data["James"][0].stockName<<data["James"][0].shares <<data["James"][0].avgBuyPrice  <<std::endl;


    while (true) {
        sleep(1);
    }

    return 0;
}
