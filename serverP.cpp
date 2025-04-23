#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "serverP.h"
#include "common_sockets.h"

using namespace serverP;

// 从文件加载所有用户的 portfolio
UserMap serverP::loadPortfolios(const std::string& filename) {
    UserMap data;
    std::ifstream infile(filename);
    std::string line;
    std::string currentUser;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (iss.eof()) {
            // 用户名行
            currentUser = token;
            data[currentUser] = Portfolio();
        } else {
            // 股票持仓行
            StockHolding holding;
            holding.stock = token;
            iss >> holding.quantity >> holding.avg_price;
            data[currentUser][holding.stock] = holding;
        }
    }

    return data;
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_P << "." << std::endl;

    UserMap portfolios = loadPortfolios(PORTFOLIO_FILE);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_P);

    while (true) {
        Optional<std::string> request = udp_recv_string(udp_sock);
        if (!request.has_value()) continue;

        std::string cmd = request.value();
        std::istringstream iss(cmd);
        std::string action, username, stock;
        int quantity;
        double price;

        iss >> action >> username >> stock >> quantity >> price;

        if (action != "buy" || username.empty() || stock.empty() || quantity <= 0 || price <= 0.0) {
            std::cerr << "[Server P] Invalid request: " << cmd << std::endl;
            continue;
        }

        // 更新用户持仓
        auto& user_port = portfolios[username];
        auto& holding = user_port[stock];

        if (holding.quantity > 0) {
            double total_value = holding.avg_price * holding.quantity + price * quantity;
            holding.quantity += quantity;
            holding.avg_price = total_value / holding.quantity;
        } else {
            holding.stock = stock;
            holding.quantity = quantity;
            holding.avg_price = price;
        }

        std::cout << "[Server P] Updated " << username << ": "
                  << stock << " " << holding.quantity << " shares at avg $" << holding.avg_price << std::endl;
    }

    return 0;
}
