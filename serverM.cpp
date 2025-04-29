#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "serverM.h"
#include "common_variables.h"
#include "common_sockets.h"

using namespace serverM;
namespace serverM {
    std::unordered_map<int, std::string> client_fd_to_user;
}

// pwd encrypt
std::string serverM::encryptPassword(const std::string& password) {
    std::string encrypted = password;
    for (char& c : encrypted) {
        if (c >= 'A' && c <= 'Z')
            c = 'A' + (c - 'A' + 3) % 26;
        else if (c >= 'a' && c <= 'z')
            c = 'a' + (c - 'a' + 3) % 26;
        else if (c >= '0' && c <= '9')
            c = '0' + (c - '0' + 3) % 10;
    }
    return encrypted;
}
void handlePosition(int client_fd, int udp_sock) {
    std::string username = client_fd_to_user[client_fd];
    std::cout << "[Server M] Received a position request from Member to check " << username << "’s gain using TCP over port " << PORT_SERVER_M_TCP << "." << std::endl;

    std::string request = "position " + username;
    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_P, request);
    std::cout << "[Server M] Forwarded the position request to server P." << std::endl;

    Optional<std::string> reply = udp_recv_string(udp_sock);
    if (!reply.has_value()) {
        tcp_send_string(client_fd, "POSITION,ERR");
        return;
    }
	std::cout << "[Server M] Received user’s portfolio from server P using UDP over "<< PORT_SERVER_M_UDP << std::endl;
    std::string replyP = reply.value();
    std::ostringstream response;
    double total_profit = 0.0;

    if (replyP.substr(0, 2) == "OK") {
        std::istringstream iss(replyP.substr(2));  // remove ok
        std::string line;
        while (std::getline(iss, line)) {
            if (line.empty()) continue;

            std::istringstream line_iss(line);
            std::string stock;
            int quantity;
            std::string at_str, avg_label;
            double avg_price;

            line_iss >> stock >> quantity >> at_str >> avg_label >> avg_price;

            udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "quote " + stock);
            Optional<std::string> quote_reply = udp_recv_string(udp_sock);
            if (!quote_reply.has_value()) {
                response << stock << " ? @ avg " << avg_price << " | P/L = ???\n";
                continue;
            }

            std::istringstream quote_iss(quote_reply.value());
            std::string stock_name;
            double current_price;
            quote_iss >> stock_name >> current_price;

            double profit = (current_price - avg_price) * quantity;
            response << stock << " " << quantity << " @ avg " << avg_price
                     << " | P/L = " << std::fixed << std::setprecision(2) << profit << "\n";
            total_profit += profit;
        }

        response << username << "’s current profit is " << std::fixed << std::setprecision(2) << total_profit << ".\n";
        std::ostringstream final_response;
        final_response << "POSITION,OK," << response.str();
        tcp_send_string(client_fd, final_response.str());
        std::cout << "[Server M] Forwarded the gain to the client." << std::endl;
    }
    else if (replyP.substr(0, 3) == "ERR") {
        tcp_send_string(client_fd, "POSITION,ERR");
    }
    else {
        tcp_send_string(client_fd, "POSITION,ERR");
    }
}

void handleQuote(int client_fd, int udp_sock,const std::string& stockName) {
    //std::cout << stockName << std::endl;
 	if (stockName == "ALL") {
        std::cout << "[Server M] Received a quote request from " << client_fd_to_user[client_fd]<<", using TCP over port " << PORT_SERVER_M_TCP << "." <<std::endl;
    } else {
         std::cout << "[Server M] Received a quote request from " << client_fd_to_user[client_fd]<<" for stock " << stockName <<
            ", using TCP over port " << PORT_SERVER_M_TCP << "."<<std::endl;
    }
  	udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "quote " + stockName);
    std::cout << AFTER_FORWARD_TO_Q << std::endl;
	Optional<std::string> quote_result = udp_recv_string(udp_sock);
    if (stockName == "ALL") {
        std::cout <<"[Server M] Received the quote response from server Q using UDP over "<< PORT_SERVER_M_UDP  <<std::endl;
    } else {
        std::cout <<"[Server M] Received the quote response from server Q for "<<stockName<<" using UDP over "<< PORT_SERVER_M_UDP <<std::endl;
    }
	std::ostringstream response;
	response << "QUOTE,";
	if (!quote_result.has_value()) {
        std::cerr << MSG_FAILED_GET_QUOTE << std::endl;
		response <<"ERROR,"<<stockName;
    } else {
		response <<"OK,"<<quote_result.value();
    }
	tcp_send_string(client_fd, response.str());
    std::cout << AFTER_FORWARD_TO_CLIENT <<std::endl;
}

void handleBuy(int client_fd, int udp_sock, const std::string& stockName, int shares) {
	std::string username = client_fd_to_user[client_fd];
    std::cout << "[Server M] Received a buy request from member " <<username<<" using TCP over port "<< PORT_SERVER_M_TCP <<". " << std::endl;
	if (stockName.empty() || shares <= 0) {
    	tcp_send_string(client_fd, MSG_BUY_INVALID);
    	return;
    }
	udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "quote " + stockName);
	std::cout << "[Server M] Sent the quote request to server Q." << std::endl;
    Optional<std::string> price_resp = udp_recv_string(udp_sock);
    if (!price_resp.has_value()) {
        tcp_send_string(client_fd, MSG_BUY_PRICE_FAIL);
        return;
    }
	std::istringstream iss(price_resp.value());
	std::string stock_reply;
	double price = -1.0;
	if (!(iss >> stock_reply >> price) || stock_reply != stockName || price <= 0.0) {
         tcp_send_string(client_fd, "BUY,ERROR,"+stockName);
         return;
	}
	//confirm
	tcp_send_string(client_fd, "BUY,CONFIRM,"+stockName+","+std::to_string(price));
	std::cout << "[Server M] Sent the buy confirmation to the client. "<<std::endl;
	auto maybe_msg = tcp_recv_string(client_fd);
    if (!maybe_msg.has_value()) {
        close(client_fd);
        return;
    }
    std::string confirm = maybe_msg.value();
	std::ostringstream final_response;
	if (confirm == "Y" || confirm == "y") {
		std::cout << "[Server M] Buy approved. " <<std::endl;
		std::ostringstream msg_to_P;
    	msg_to_P << "buy " << client_fd_to_user[client_fd] << " " << stockName << " " << shares << " " << price;
		std::cout << "[Server M] Forwarded the buy confirmation response to Server P. " << std::endl;
    	udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_P, msg_to_P.str());
		udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "advance " + stockName);
    	std::cout << "[Server M] Sent a time forward request for "<<stockName<<"." << std::endl;
		final_response <<"BUY,OK,"<<stockName<<","<<price<<","<<shares;
		std::cout << "[Server M] Forwarded the buy result to the client. " << std::endl;
	}else{
		std::cout << "[Server M] Buy denied.  " <<std::endl;
		final_response <<"BUY,ERROR";
	}


	// final response
 	tcp_send_string(client_fd, final_response.str());
}
void handleSell(int client_fd, int udp_sock, const std::string& stockName, int shares) {
	std::string username = client_fd_to_user[client_fd];
    std::cout << "[Server M] Received a sell request from member " << username
              << " using TCP over port " << PORT_SERVER_M_TCP << "." << std::endl;

	//std::cout << stockName << std::endl;
	// get price of Q
	udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "quote " + stockName);
    std::cout << "[Server M] Sent the quote request to server Q." << std::endl;
	Optional<std::string> price_resp = udp_recv_string(udp_sock);
	std::string content = price_resp.value();

	if (content.find("does not exist") != std::string::npos) {
   		tcp_send_string(client_fd, "SELL,ERROR,stock name does not exist");
		return;
	}
	std::istringstream price_iss(price_resp.value());
    std::string stock_reply;
    double price;
    price_iss >> stock_reply >> price;

	//std::cout <<price << std::endl;
	//chaeck enough shares
    std::ostringstream check_msg;
    check_msg << "check " << username << " " << stockName << " " << shares;
    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_P, check_msg.str());
    std::cout << "[Server M] Forwarded the sell request to server P." << std::endl;

    Optional<std::string> check_resp = udp_recv_string(udp_sock);
    if (!check_resp.has_value()) {
        tcp_send_string(client_fd, "ERR sell no response from ServerP");
        return;
    }
    std::cout << "[Server M] Received user's portfolio from server P using UDP over "
              << PORT_SERVER_M_UDP << "." << std::endl;

    if (check_resp.value() != "OK") {
        tcp_send_string(client_fd, "SELL,ERROR,"+username+" does not have enough shares of " + stockName);
        return;
    }

	//confirm
	std::ostringstream confirm_msg;
    confirm_msg << "SELL,CONFIRM," << stockName << "," << price ;
    tcp_send_string(client_fd, confirm_msg.str());
	std::cout << "[Server M] Forwarded the sell confirmation to the client."<<std::endl;

	Optional<std::string> client_confirm = tcp_recv_string(client_fd);
    if (client_confirm.has_value() && client_confirm.value() != "Y" && client_confirm.value() != "y") {
		tcp_send_string(client_fd, "SELL,ERROR");
		return;
    }
	std::ostringstream sell_msg;
    sell_msg << "sell " << username << " " << stockName << " " << shares << " " << price << " Y";
    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_P, sell_msg.str());
  	udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_Q, "advance " + stockName);
    std::cout << "[Server M] Forwarded the sell confirmation response to Server P." << std::endl;
	std::ostringstream final_response;
	final_response <<"SELL,OK,"<<username<<","<<shares<<","<<stockName;
	// final response
 	tcp_send_string(client_fd, final_response.str());
}


// login
void serverM::handle_single_client(int client_fd, int udp_sock) {
    a:
    auto maybe_msg = tcp_recv_string(client_fd);
    if (!maybe_msg.has_value()) {
        close(client_fd);
        return;
    }
    std::string received = maybe_msg.value();

    auto comma_pos = received.find(',');
    std::string username = received.substr(0, comma_pos);
    std::string password = received.substr(comma_pos + 1);

    std::cout << RECEIVED_FROM_CLIENT << username << " and password " << MSG_PASSWORD_MASK << "." << std::endl;

    std::string encrypted = encryptPassword(password);
    std::string messageToA = username + "," + encrypted;

    udp_send_string(udp_sock, LOCALHOST, PORT_SERVER_A, messageToA);
    std::cout << SENT_TO_SERVERA << std::endl;

    auto authResultOpt = udp_recv_string(udp_sock);
    if (!authResultOpt.has_value()) {
        std::cerr << MSG_FAILED_FROM_SERVERA << std::endl;
        close(client_fd);
        return;
    }

    std::string authResult = authResultOpt.value();
    std::cout << RECEIVED_FROM_SERVERA << PORT_SERVER_M_UDP << std::endl;

    tcp_send_string(client_fd, authResult);
    std::cout << SENT_TO_CLIENT << PORT_SERVER_M_TCP << std::endl;

    if (authResult == MSG_AUTH_GRANTED) {
        client_fd_to_user[client_fd] = username;
        handle_phase3_commands(client_fd, udp_sock, username);
    }
    else{
        goto a;
    }


    client_fd_to_user.erase(client_fd);
    close(client_fd);
}
void serverM::handle_phase3_commands(int client_fd, int udp_sock, const std::string& username) {
    while (true) {
        Optional<std::string> cmd = tcp_recv_string(client_fd);
        if (!cmd.has_value()) {
            std::cerr << MSG_CLIENT_DISCONNECT << std::endl;
            break;
        }

        std::string input = cmd.value();
		std::string command = cmd.value();
		std::istringstream iss(input);
    	std::string new_command, arg1, arg2;
    	iss >> new_command >> arg1 >> arg2;

        if (new_command == "exit") {
            std::cout << MSG_CLIENT_EXITED << std::endl;
            break;
        }

        if (new_command == "quote") {
        	if (arg1.empty()) handleQuote(client_fd, udp_sock, "ALL");
        	else handleQuote(client_fd, udp_sock, arg1);
        }else if (new_command == "buy") {
        	if (!arg1.empty() && !arg2.empty()) handleBuy(client_fd, udp_sock,arg1, std::stoi(arg2));
    	}else if (new_command == "position") {
            handlePosition(client_fd, udp_sock);
        }else if (new_command == "sell"){
			if (!arg1.empty() && !arg2.empty()) handleSell(client_fd, udp_sock,arg1, std::stoi(arg2));
		}
        else {
            tcp_send_string(client_fd, MSG_UNKNOWN_COMMAND);
        }
    }
}

int main() {
    std::cout << BOOTUP_MESSAGE << PORT_SERVER_M_UDP << "." << std::endl;
    std::cout << TCP_LISTEN_MESSAGE << PORT_SERVER_M_TCP << "." << std::endl;

    int tcp_sock = create_tcp_server_socket(LOCALHOST, PORT_SERVER_M_TCP);
    int udp_sock = create_udp_server_socket(LOCALHOST, PORT_SERVER_M_UDP);

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(tcp_sock, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        serverM::handle_single_client(client_fd, udp_sock);
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
