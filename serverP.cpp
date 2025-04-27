#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "serverP.h"
#include "common_sockets.h"

using namespace serverP;

static std::string trim(const std::string& s) {
    auto l = s.find_first_not_of(" \t\r\n");
    if (l == std::string::npos) return "";
    auto r = s.find_last_not_of(" \t\r\n");
    return s.substr(l, r - l + 1);
}

// === GLOBAL PORTFOLIO ===
UserMap portfolios;

void handle_buy(const std::string& username, const std::string& stock, int quantity, double price, int sockfd) {
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

void handle_sell(const std::string& username, const std::string& stock, int quantity, double price, const std::string& confirm, int sockfd) {
    std::cout << "[Server P] Received a sell request from the main server." << std::endl;

    if (confirm != "Y") {
        std::cout << "[Server P] Sell denied." << std::endl;
        udp_send_string(sockfd, LOCALHOST, PORT_SERVER_M_UDP, "ERR sell denied by user");
        return;
    }
    std::cout << "[Server P] User approves selling the stock." << std::endl;

    auto& user_port = portfolios[username];
    StockHolding& holding = user_port[stock];
    holding.quantity -= quantity;
    if (holding.quantity <= 0) {
        user_port.erase(stock);
    }

    std::cout << "[Server P] Successfully sold " << quantity << " shares of "
              << stock << " and updated " << username << "’s portfolio." << std::endl;

}


bool hasSufficientShares(const std::string& username,
                         const std::string& stock,
                         int quantity) {
    auto userIt = portfolios.find(username);
    if (userIt == portfolios.end()) {
        return false;
    }
    auto stockIt = userIt->second.find(stock);
    return (stockIt != userIt->second.end()
            && stockIt->second.quantity >= quantity);
}

// 打印当前 portfolios 内容
void printPortfolios(const UserMap& portfolios) {
    for (const auto& user_pair : portfolios) {
        const std::string& username = user_pair.first;
        const Portfolio& portfolio = user_pair.second;
        std::cout << username << ":" << std::endl;
        for (const auto& stock_pair : portfolio) {
            const StockHolding& holding = stock_pair.second;
            std::cout << "  " << holding.stock
                      << " " << holding.quantity
                      << " @ avg " << holding.avg_price << std::endl;
        }
    }
}


UserMap loadPortfolios(const std::string& filename) {
    UserMap data;
    std::ifstream infile(filename);
    std::string line, currentUser;

    while (std::getline(infile, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;            // 跳过空行

        std::istringstream iss(t);
        std::vector<std::string> parts;
        std::string tok;
        while (iss >> tok) parts.push_back(tok);

        if (parts.size() == 1) {
            // 只有用户名
            currentUser = parts[0];
            data[currentUser] = Portfolio();
        } else if (parts.size() == 3) {
            // 股票 持仓数 平均价
            StockHolding h;
            h.stock     = parts[0];
            h.quantity  = std::stoi(parts[1]);
            h.avg_price = std::stod(parts[2]);
            data[currentUser][h.stock] = h;
        } else {
            std::cerr << "[loadPortfolios] 无法识别的行: " << line << std::endl;
        }
    }
    return data;
}

// 查询用户持仓
void handle_position(const std::string& username, int udp_sock) {
    if (username.empty()) {
        udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, "ERR invalid username");
        return;
    }

    auto it = portfolios.find(username);
    if (it == portfolios.end() || it->second.empty()) {
        udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, "ERR no data");
        return;
    }

    std::ostringstream response;
    response << "OK";

    for (const auto& pair : it->second) {
        const StockHolding& holding = pair.second;
        response << holding.stock << " " << holding.quantity
                 << " @ avg " << holding.avg_price << "\n";
    }

    std::cout << "[Server P] Sent position for user: " << username << std::endl;
    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, response.str());
	std::cout << response.str() << std::endl;
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_P << "." << std::endl;

    portfolios = loadPortfolios(PORTFOLIO_FILE);

    //printPortfolios(portfolios);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_P);

    while (true) {
        Optional<std::string> request = udp_recv_string(udp_sock);
        if (!request.has_value()) continue;

        std::istringstream iss(request.value());
        std::string action, username, stock, confirm;
        int quantity = 0;
        double price = 0.0;

        iss >> action;

        if (action == "buy") {
            iss >> username >> stock >> quantity >> price;
            handle_buy(username, stock, quantity, price, udp_sock);

        } else if (action == "sell") {
            // NEW: 接收来自ServerM的确认信息
            iss >> username >> stock >> quantity >> price >> confirm;
            handle_sell(username, stock, quantity, price, confirm, udp_sock);

        } else if (action == "check") {
            iss >> username >> stock >> quantity;
            bool ok = hasSufficientShares(username, stock, quantity);
            if (ok) {
                std::cout << "[Server P] Stock " << stock
                          << " has sufficient shares in " << username
                          << "’s portfolio. Requesting user confirmation."
                          << std::endl;
                udp_send_string(udp_sock, LOCALHOST,
                                PORT_SERVER_M_UDP, "OK");
            } else {
                std::cout << "[Server P] Stock " << stock
                          << " does not have enough shares in " << username
                          << "’s portfolio. Unable to sell "
                          << quantity << " shares." << std::endl;
                udp_send_string(udp_sock, LOCALHOST,
                                PORT_SERVER_M_UDP, "ERR");
            }
        } else if (action == "position") {
            iss >> username;
            handle_position(username, udp_sock);
        } else {
            std::cerr << "[Server P] Unknown command: " << request.value() << std::endl;
        }
        //printPortfolios(portfolios);
    }

    close(udp_sock);
    return 0;
}
