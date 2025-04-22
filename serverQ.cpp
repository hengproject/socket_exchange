#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "serverQ.h"

using namespace serverQ;

QuoteMap serverQ::loadQuotes(const std::string& filename) {
    QuoteMap quotes;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string stock;
        double price;
        iss >> stock;

        StockQuote quote;
        while (iss >> price) {
            quote.prices.push_back(price);
        }

        if (!quote.prices.empty()) {
            quotes[stock] = quote;
        }
    }

    return quotes;
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_Q << "." << std::endl;

    QuoteMap data = loadQuotes("quotes.txt");
    std::cout << "[Server Q] Loaded quotes for " << data.size() << " stocks from quotes.txt" << std::endl;

    while (true) {
        sleep(1);
    }

    return 0;
}
