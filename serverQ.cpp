#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "serverQ.h"
#include "common_sockets.h"

using namespace serverQ;

// load quote.txt
QuoteMap serverQ::loadQuotes(const std::string& filename) {
    QuoteMap quotes;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string stock;
        double price;
        iss >> stock;

        QuoteInfo info;
        while (iss >> price) {
            info.prices.push_back(price);
        }

        if (!info.prices.empty()) {
            quotes[stock] = info;
        }
    }

    return quotes;
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_Q << "." << std::endl;

    QuoteMap quotes = loadQuotes(QUOTE_FILE);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_Q);

    while (true) {
        Optional<std::string> request = udp_recv_string(udp_sock);
        if (!request.has_value()) {
            std::cerr << "[Server Q] Failed to receive quote request." << std::endl;
            continue;
        }

        std::string command = request.value();
        std::istringstream iss(command);
        std::string cmd, stock;
        iss >> cmd >> stock;

        std::ostringstream response;

        if (cmd == CMD_QUOTE) {
            if (stock.empty()) {
                std::cout << RECIEVED_GENERAL_QOUTE<< std::endl;
                // quote all
                for (const auto& pair : quotes) {
                    response << pair.first << " " << pair.second.current_price() << "\n";
                }
                std::cout << GENERAL_QUOTE_RET<< std::endl;
            } else {
                std::cout << RECIEVED_SPECIFIC_QOUTE<< stock <<"."<<std::endl;
                // quote one
                auto it = quotes.find(stock);
                if (it != quotes.end()) {
                    response << stock << " " << it->second.current_price() << "\n";
                    std::cout << SPECIFIC_QOUTE_RET<< stock <<"."<<std::endl;
                } else {
                    response << stock << RESP_STOCK_404 << "\n";
                }

            }
        }else if (cmd == CMD_ADVANCE) {
            auto it = quotes.find(stock);
            if (it != quotes.end()) {
                std::cout << "[Server Q] Received a time forward request for " << stock
             << ", the current price of that stock is " << it->second.current_price()
             << " at time " << it->second.current_index << "." << std::endl;
                it->second.advance();
                response << RESP_ADVANCE_OK << stock << "\n";
            } else {
                response << RESP_ADVANCE_FAIL << stock << "\n";
            }
        }
        else {
            response << RESP_UNKNOWN_COMMAND;
        }

        udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_M_UDP, response.str());
    }

    close(udp_sock);
    return 0;
}
