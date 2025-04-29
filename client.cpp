#include <iostream>
#include <tuple>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "common_variables.h"
#include "common_sockets.h"
#include "client.h"

std::string username;
int sockfd;
void enter_command_loop(int sockfd,const std::string& username);
bool confirm_YN(const std::string& prompt);
int getClientPort(int sockfd);
void new_req();
std::tuple<std::string, std::string, std::string> parseMessage(const std::string& message) {
    size_t firstComma = message.find(',');
    size_t secondComma = message.find(',', firstComma + 1);

    std::string type, status, content;
    if (firstComma != std::string::npos) {
        type = message.substr(0, firstComma);
    }
    if (secondComma != std::string::npos) {
        status = message.substr(firstComma + 1, secondComma - firstComma - 1);
    }
    if (secondComma != std::string::npos) {
        content = message.substr(secondComma + 1);
    }

    return std::make_tuple(type, status, content);
}

void new_req(){
	std::cout << "----- Start a new request -----" << std::endl;
}


void QuoteResponseHandler(const std::string& status, const std::string& content) {
    //std::cout << "[Client] Received the response from the main server using TCP over port <client port number>. " << std::endl;
    if (status == "OK") {
        std::cout << content << std::endl;
    } else if (status == "ERROR") {
        std::cout << content <<" does not exist. Please try again. "<< std::endl;
    }
	new_req();
}

void PositionResponseHandler(const std::string& status, const std::string& content) {
    //std::cout << "[Client] Received the response from the main server using TCP over port <client port number>." << std::endl;
	if (status == "OK") {
        std::cout << content << std::endl;
    } else if (status == "ERROR") {
        std::cout << username <<"’s current profit is 0.0. "<< std::endl;
    }
	new_req();
    return;
}

void BuyResponseHandler(const std::string& status, const std::string& content) {
    //std::cout << "[Client] Received the response from the main server using TCP over port <client port number>." << std::endl;

    if (status == "ERROR") {
        if (content.empty()) {
            std::cout << "[Client] Buy request denied by the user." << std::endl;
        } else {
            std::cout << "[Client] ERROR: stock name " << content << " does not exist. Please check again." << std::endl;
        }
    }
    else if (status == "CONFIRM") {
        size_t comma = content.find(',');
        std::string stock = content.substr(0, comma);
        std::string price = content.substr(comma + 1);
        std::cout << "[Client] " << stock << "'s current price is " << price << ". Proceed to buy? (Y/N)" << std::endl;
	}
    else if (status == "OK") {
        size_t first = content.find(',');
        size_t second = content.find(',', first + 1);
        std::string stock = content.substr(0, first);
        std::string price = content.substr(first + 1, second - first - 1);
        std::string shares = content.substr(second + 1);
        std::cout << "[Client] " << username << " successfully bought "<< shares <<" shares of " << stock << "." << std::endl;
		new_req();
    }
}

void SellResponseHandler(const std::string& status, const std::string& content) {
    if (status == "OK") {
        // content: username,number_of_shares,stock_name
        std::istringstream iss(content);
        std::string username, number_of_shares, stock_name;
        getline(iss, username, ',');
        getline(iss, number_of_shares, ',');
        getline(iss, stock_name, ',');

        std::cout << "[Client] " << username << " successfully sold " << number_of_shares
                  << " shares of " << stock_name << "." << std::endl;
        		new_req();

    } else if (status == "ERROR") {
		if (content.empty()) {
			std::cout << "[Client] Sell request denied by the user." << std::endl;
		}
        else if (content == "stock name does not exist") {
            std::cout << "[Client] Error: stock name does not exist. Please check again." << std::endl;
        } else {
            // content format: username does not have enough shares of ,stock_name
            std::istringstream iss(content);
            std::string msg, stock_name;
            getline(iss, msg, ',');
            getline(iss, stock_name, ',');
            std::cout << "[Client] Error: " << msg << stock_name << " to sell. Please try again" << std::endl;
        }
		new_req();

    } else if (status == "CONFIRM") {
        // content format: stock_name,current_price
        std::istringstream iss(content);
        std::string stock_name, current_price;
        getline(iss, stock_name, ',');
        getline(iss, current_price, ',');

        std::cout << "[Client] " << stock_name << "'s current price is " << current_price
                  << ". Proceed to sell? (Y/N)" << std::endl;
    }
}
void HandleServerReply(const std::string& type, const std::string& status, const std::string& content) {
    if (type == "QUOTE") {
        QuoteResponseHandler(status, content);
    } else if (type == "POSITION") {
        PositionResponseHandler(status, content);
    } else if (type == "BUY") {
        BuyResponseHandler(status, content);
    } else if (type == "SELL") {
        SellResponseHandler(status, content);
    } else {
        std::cerr << "Unknown response type: " << type << std::endl;
    }
}

int main() {
    std::cout << client::BOOTUP_MESSAGE << std::endl;

    // create tcp client socket
    sockfd = create_tcp_client_socket(LOCALHOST, PORT_SERVER_M_TCP);

    std::cout << client::LOGIN_PROMPT << std::endl;

    while (true) {

       std::string username, password;
       std::cout << client::PROMPT_USERNAME;
       std::getline(std::cin, username);
       std::cout << client::PROMPT_PASSWORD;
       std::getline(std::cin, password);
       if (username.empty() || password.empty()) {
           std::cerr << "[Client] Username and password must not be empty." << std::endl;
           continue;
       }
       if (username.length() > 51 || password.length() > 51) {
          std::cerr << "[Client] Username/password too long." << std::endl;
          continue;
       }

       std::string message = username + "," + password;
        //username="*";
        //std::string message = "*,*";
        tcp_send_string(sockfd, message);

        auto reply = tcp_recv_string(sockfd);
        if (!reply.has_value()) {
            std::cerr << client::CONNECTION_ERROR << std::endl;
            break;
        }

        std::string response = reply.value();
        if (response == MSG_AUTH_GRANTED) {
            std::cout << client::PROMPT_CORRECT_CREDENTIAL << std::endl;
            enter_command_loop(sockfd,username);
            break;
        } else {
            std::cout << client::PROMPT_WRONG_CREDENTIAL << std::endl;
        }
    }

    close(sockfd);
    return 0;
}
int getClientPort(int sockfd) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr*)&addr, &addr_len) == -1) {
        perror("getsockname failed");
        return -1;
    }

    return ntohs(addr.sin_port);
}

bool validateInput(const std::string& input) {
    bool ok = true;
    if (input.find("buy") == 0) {
        size_t firstSpace = input.find(' ');
        size_t secondSpace = input.find(' ', firstSpace + 1);
        size_t thirdSpace = input.find(' ', secondSpace + 1);

        ok = (firstSpace != std::string::npos &&
              secondSpace != std::string::npos &&
              thirdSpace == std::string::npos);

        if (!ok) {
            std::cout << "[Client] Error: stock name/shares are required. Please specify a stock name to buy." << std::endl;
        }
    } else if (input.find("sell") == 0) {
        size_t firstSpace = input.find(' ');
        size_t secondSpace = input.find(' ', firstSpace + 1);
        size_t thirdSpace = input.find(' ', secondSpace + 1);

        ok = (firstSpace != std::string::npos &&
              secondSpace != std::string::npos &&
              thirdSpace == std::string::npos);

        if (!ok) {
            std::cout << "[Client] Error: stock name/shares are required. Please specify a stock name to sell." << std::endl;
        }
	}else if (input.find("position") == 0) {
			std::cout << "[Client] "<<username<<" sent a position request to the main server. " << std::endl;
    }else if (input.find("quote") == 0){
		std::cout<<"[Client] Sent a quote request to the main server. " <<std::endl;
	}
    return ok;
}


void enter_command_loop(int sockfd, const std::string& username) {
    while (true) {
        std::cout << client::PROMPT_COMMAND;
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << client::MSG_GOODBYE << std::endl;
            break;
        }
		if (validateInput(command)){
        	tcp_send_string(sockfd, command);
		}else{
			std::cout << "----- Start a new request -----" << std::endl;
			continue;
		}
        //std::cout << client::MSG_SENT_COMMAND << std::endl;

        Optional<std::string> reply = tcp_recv_string(sockfd);
        if (!reply.has_value()) {
            std::cerr << client::CONNECTION_ERROR << std::endl;
            break;
        }

        std::string server_reply = reply.value();
        std::cout << "[Client] Received the response from the main server using TCP over port "<< getClientPort(sockfd) <<". " << std::endl;
		//std::cout << server_reply << std::endl;
		auto message = parseMessage(server_reply);
		HandleServerReply(std::get<0>(message),std::get<1>(message),std::get<2>(message));
        //std::cout << "----- Start a new request -----" << std::endl;
    }
}
