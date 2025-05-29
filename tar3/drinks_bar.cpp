#include "drinks_bar.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <sstream>
#include <signal.h>

int server_fd = -1, udp_fd = -1;

void cleanup(int signum) {
if (server_fd != -1) close(server_fd);
if (udp_fd != -1) close(udp_fd);
std::cout << "\nSockets closed. Exiting." << std::endl;
exit(0);
}

void printStock() {
    std::cout << "=== Current Stock ===" << std::endl;
    std::cout << "Total atoms   : " << atom_count << std::endl;
    std::cout << "Carbon (C)    : " << carbon_count << std::endl;
    std::cout << "Hydrogen (H)  : " << hydrogen_count << std::endl;
    std::cout << "Oxygen (O)    : " << oxygen_count << std::endl;
    std::cout << "=====================" << std::endl;
}

bool isInteger(const std::string& s) {
    try {
        size_t pos;
        std::stoi(s, &pos);
    
        return pos == s.length();
    } catch (std::invalid_argument& e) {
        return false; 
    } catch (std::out_of_range& e) {
        return false; 
    }
}

bool deliver_water(long long count){
    if (count <= 0) {
        std::cout << "Error: Invalid count for water delivery." << std::endl;
        return false;
    }
    if (hydrogen_count < 2 * count || oxygen_count < count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " water molecules." << std::endl;
        return false;
    }
    hydrogen_count -= 2 * count;
    oxygen_count -= count;
    atom_count -= 3 * count; // 2H + O = H2O
    std::cout << "Delivered " << count << " water molecules." << std::endl;
    // printStock();
    return true;
}

bool deliver_carbon_dioxide(long long count) {
    if (count <= 0) {
        std::cout << "Error: Invalid count for carbon dioxide delivery." << std::endl;
        return false;
    }
    if (carbon_count < count || oxygen_count < 2 * count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " carbon dioxide molecules." << std::endl;
        return false;
    }
    carbon_count -= count;
    oxygen_count -= 2 * count;
    atom_count -= 3 * count; //CO2
    std::cout << "Delivered " << count << " carbon dioxide molecules." << std::endl;
    //printStock();
    return true;
}

bool deliver_alcohol(long long count) {
    if (count <= 0) {
        std::cout << "Error: Invalid count for alcohol delivery." << std::endl;
        return false;
    }
    if (carbon_count < 2*count || hydrogen_count <  6*count || oxygen_count < count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " alcohol molecules." << std::endl;
        return false;
    }
    carbon_count -= 2*count;
    hydrogen_count -= 6 * count;
    oxygen_count -= count;
    atom_count -= 9 * count; //C2H6O
    std::cout << "Delivered " << count << " alcohol molecules." << std::endl;
    //printStock();
    return true;
}

bool deliver_glucose(long long count) {
    if (count <= 0) {
        std::cout << "Error: Invalid count for glucose delivery." << std::endl;
        return false;
    }
    if (carbon_count < 6*count || hydrogen_count < 12 * count || oxygen_count < 6*count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " glucose molecules." << std::endl;
        return false;
    }
    carbon_count -= 6*count;
    hydrogen_count -= 12*count;
    oxygen_count -= 6*count;
    atom_count -= 24 * count; //C6H12O6
    std::cout << "Delivered " << count << " glucose molecules." << std::endl;
    //printStock();
    return true;
}
     
long long num_of_soft_drinks(){
    long long tmp_car=carbon_count;
    long long tmp_hyd=hydrogen_count;
    long long tmp_oxy=oxygen_count;
    long long count = 0;

    while(true){
        //water
        tmp_hyd -= 2;
        tmp_oxy -= 1;
        //carbon dioxide
        tmp_car -= 1;
        tmp_oxy -= 2;
        //glucose
        tmp_car -= 6;
        tmp_hyd -= 12;
        tmp_oxy -= 6;

        if(tmp_hyd < 0 || tmp_oxy < 0 || tmp_car < 0){
            break;
        }
        count++;
    }
    std::cout << "Number of soft drinks that can be created: " << count << std::endl;
    return count;
}


long long num_of_vodka(){
    long long tmp_car=carbon_count;
    long long tmp_hyd=hydrogen_count;
    long long tmp_oxy=oxygen_count;
    long long count = 0;

        while(true){
        //water
        tmp_hyd -= 2;
        tmp_oxy -= 1;
        //alcohol
        tmp_car -= 2;
        tmp_hyd -= 6;
        tmp_oxy -= 1;
        //glucose
        tmp_car -= 1;
        tmp_oxy -= 2;

        if(tmp_hyd < 0 || tmp_oxy < 0 || tmp_car < 0){
            break;
        }
        count++;
    }
    std::cout << "Number of champagne that can be created: " << count << std::endl;
    return count;
}

long long num_of_champagne(){
    long long tmp_car=carbon_count;
    long long tmp_hyd=hydrogen_count;
    long long tmp_oxy=oxygen_count;
    long long count = 0;

        while(true){
        //water
        tmp_hyd -= 2;
        tmp_oxy -= 1;
        //alcohol
        tmp_car -= 2;
        tmp_hyd -= 6;
        tmp_oxy -= 1;
        //carbon dioxide
        tmp_car -= 6;
        tmp_hyd -= 12;
        tmp_oxy -= 6;

        if(tmp_hyd < 0 || tmp_oxy < 0 || tmp_car < 0){
            break;
        }
        count++;
    }
    std::cout << "Number of vodka that can be created: " << count << std::endl;
    return count;
}

void run_server(int port_tcp, int port_udp) {
    int server_fd, new_socket, max_sd, activity, valread, sd;
    int client_socket[FD_SETSIZE] = {0};
    struct sockaddr_in address;
    fd_set readfds;

    // Create a TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw std::runtime_error("socket failed");
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("setsockopt failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_tcp);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw std::runtime_error("bind failed");
    }
    if (listen(server_fd, 10) < 0) {
        throw std::runtime_error("listen failed");
    }
    std::cout << "Server listening over tcp on port " << port_tcp << std::endl;

    // Create a UDP socket
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        throw std::runtime_error("UDP socket failed");
    }
    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(port_udp);
    if (bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        throw std::runtime_error("UDP bind failed");
    }
    std::cout << "Server listening over udp on port " << port_udp << std::endl;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(udp_fd, &readfds);
        FD_SET(0, &readfds); // Add stdin to the set for reading
       
        max_sd = server_fd;
        if (udp_fd > max_sd) max_sd = udp_fd;
        if (max_sd < 0) max_sd = 0;

        for (int i = 0; i < FD_SETSIZE; i++) {
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0 && errno != EINTR) {
            std::cout << "select error" << std::endl;
            break;
        }

        // Handle new TCP connections
        if (FD_ISSET(server_fd, &readfds)) {
            socklen_t addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
                std::cout << "accept error" << std::endl;
                continue;
            }
            for (int i = 0; i < FD_SETSIZE; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
            std::cout << "New connection, socket fd: " << new_socket << std::endl;
        }

        if (FD_ISSET(0, &readfds)) {
        std::string input;
        std::getline(std::cin, input);
            if (input == "exit") {
                std::cout << "Exiting server..." << std::endl;
                break;
            }
            if(input == "GEN SOFT DRINK"){
                std::cout << "Received input: " << input << std::endl;
                num_of_soft_drinks();
            }
            else if (input == "GEN VODKA"){
                std::cout << "Received input: " << input << std::endl;
                num_of_vodka();
            }
            else if(input == "GEN CHAMPAGNE"){
            std::cout << "Received input: " << input << std::endl;
                num_of_champagne();
            }
            else {
                std::cout << "Unknown command: " << input << std::endl;
            }
            
        }
        // Handle UDP activity

            if (FD_ISSET(udp_fd, &readfds)) {
            char udp_buffer[1024] = {0};
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            ssize_t udp_len = recvfrom(udp_fd, udp_buffer, sizeof(udp_buffer) - 1, 0,
                                    (struct sockaddr*)&client_addr, &addrlen);
            if (udp_len > 0) {
                udp_buffer[udp_len] = '\0';
                std::cout << "Received UDP: " << udp_buffer << std::endl;
                std::istringstream iss(udp_buffer);
                std::string line;
                while (std::getline(iss, line)) {
                    std::istringstream line_stream(line);
                    std::string cmd, molecule, amount_str;
                    line_stream >> cmd >> molecule >> amount_str;
                    if (cmd == "DELIVER" && isInteger(amount_str)) {
                        int amount = std::stoi(amount_str);
                        std::string response;
                        if (molecule == "WATER") {
                            if (!deliver_water(amount)) {
                                std::cout << "Failed to deliver water." << std::endl;
                                response = "FAILED TO DELIVER WATER\n";
                            } else {
                                response = "SUCCESSFULLY DELIVERED WATER\n";
                                std::cout << "Successfully delivered water." << std::endl;
                            }
                        } else if (molecule == "CARBON_DIOXIDE") {
                            if (!deliver_carbon_dioxide(amount)) {
                                std::cout << "Failed to deliver carbon dioxide." << std::endl;
                                response = "FAILED TO DELIVER CARBON_DIOXIDE\n";
                            } else {
                                response = "SUCCESSFULLY DELIVERED CARBON_DIOXIDE\n";
                            }
                        } else if (molecule == "ALCOHOL") {
                            if (!deliver_alcohol(amount)) {
                                std::cout << "Failed to deliver alcohol." << std::endl;
                                response = "FAILED TO DELIVER ALCOHOL\n";
                            } else {
                                response = "SUCCESSFULLY DELIVERED ALCOHOL\n";
                            }
                        } else if (molecule == "GLUCOSE") {
                            if (!deliver_glucose(amount)) {
                                std::cout << "Failed to deliver glucose." << std::endl;
                                response = "FAILED TO DELIVER GLUCOSE\n";
                            } else {
                                response = "SUCCESSFULLY DELIVERED GLUCOSE\n";
                            }
                        } else {
                            std::cout << "Error: Unknown molecule type: " << molecule << std::endl;
                            response = "ERROR: UNKNOWN MOLECULE\n";
                        }

                        ssize_t sent = sendto(udp_fd, response.c_str(), response.size(), 0,
                                            (struct sockaddr*)&client_addr, addrlen);
                        if (sent < 0) {
                            std::cerr << "Send failed" << std::endl;
                        }
                        printStock();
                    } else if (!cmd.empty()) {
                        std::cout << "Error: Invalid command: " << line << std::endl;
                        std::string response = "ERROR: INVALID COMMAND\n";
                        sendto(udp_fd, response.c_str(), response.size(), 0,
                            (struct sockaddr*)&client_addr, addrlen);
                    }
                }
            }
        }

        // Handle TCP client activity
        for (int i = 0; i < FD_SETSIZE; i++) {
            sd = client_socket[i];
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buffer[1024] = {0};
                valread = read(sd, buffer, sizeof(buffer) - 1);
                if (valread <= 0) {
                    close(sd);
                    client_socket[i] = 0;
                    std::cout << "Client disconnected, socket fd: " << sd << std::endl;
                } else {
                    buffer[valread] = '\0';
                    std::istringstream iss(buffer);
                    std::string line;
                    while (std::getline(iss, line)) {
                        std::istringstream line_stream(line);
                        std::string cmd, atom, amount_str;
                        line_stream >> cmd >> atom >> amount_str;
                        if (cmd == "ADD" && isInteger(amount_str)) {
                            int amount = std::stoi(amount_str);
                            if (atom == "CARBON") {
                                add_carbon(amount);
                            } else if (atom == "HYDROGEN") {
                                add_hydrogen(amount);
                            } else if (atom == "OXYGEN") {
                                add_oxygen(amount);
                            } else {
                                std::cout << "Error: Unknown atom type: " << atom << std::endl;
                                continue;
                            }
                            printStock();
                        } else if (!cmd.empty()) {
                            std::cout << "Error: Invalid command: " << line << std::endl;
                        }
                    }
                }
            }
        }
    }
    close(server_fd);
    close(udp_fd);
}



int main(int argc, char *argv[]) { // Main function to start the server
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <portTCP> <portTCP>" << std::endl;
        return 0;
    }
    signal(SIGINT, cleanup);
    int port_tcp = std::atoi(argv[1]);
    int port_udp = std::atoi(argv[2]);
    if (port_tcp <= 0 || port_tcp > 65535) {
        std::cout << "Invalid port number. Please provide a port between 1 and 65535." << std::endl;
        return 0;
    }
    if (port_udp <= 0 || port_udp > 65535) {
        std::cout << "Invalid port number. Please provide a port between 1 and 65535." << std::endl;
        return 0;
    }
    try {
        run_server(port_tcp, port_udp);
    } catch (const std::runtime_error &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 1;
}