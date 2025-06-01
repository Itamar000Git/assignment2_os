#include "atom_warehouse.hpp"
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


    void run_server(int port) {
        
        int server_fd, new_socket, max_sd, activity, valread, sd;
        int client_socket[FD_SETSIZE] = {0};
        struct sockaddr_in address;
        fd_set readfds;

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // Create socket
            throw std::runtime_error("socket failed");
        }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { // Set socket options
            throw std::runtime_error("setsockopt failed");
        }

        address.sin_family = AF_INET; // Set address family to IPv4
        address.sin_addr.s_addr = INADDR_ANY; // Bind to any address
        address.sin_port = htons(port); // Set port number

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // Bind socket to address
            throw std::runtime_error("bind failed");
        }

        if (listen(server_fd, 10) < 0) { // Listen for incoming connections
            throw std::runtime_error("listen failed");
        }

        std::cout << "Server listening on port " << port << std::endl;

        while (true) {
            FD_ZERO(&readfds); // Clear the set of file descriptors
            FD_SET(server_fd, &readfds);  // Add server socket to the set
            max_sd = server_fd; // Initialize max_sd to server_fd

            for (int i = 0; i < FD_SETSIZE; i++) { // Add client sockets to the set if they are valid
                sd = client_socket[i];
                if (sd > 0)
                    FD_SET(sd, &readfds);
                if (sd > max_sd)
                    max_sd = sd;
            }

            activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr); // Wait for activity on the sockets
            if (activity < 0 && errno != EINTR) { // Check for errors in select
                std::cout << "select error" << std::endl;
                break;
                
            }
                

            if (FD_ISSET(server_fd, &readfds)) { // Check if there is activity on the server socket
                socklen_t addrlen = sizeof(address);
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
                    std::cout << "accept error" << std::endl;
                    continue;
                }
                for (int i = 0; i < FD_SETSIZE; i++) { // Find an empty slot for the new client socket
                    if (client_socket[i] == 0) {
                        client_socket[i] = new_socket;
                        break;
                    }
                }
                std::cout << "New connection, socket fd: " << new_socket << std::endl;
            }

        
            for (int i = 0; i < FD_SETSIZE; i++) { // Check for activity on each client socket
                sd = client_socket[i];
                if (sd > 0 && FD_ISSET(sd, &readfds)) { // If the socket is valid and has activity
                    char buffer[1024] = {0};
                    valread = read(sd, buffer, sizeof(buffer) - 1); // Read data from the socket
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
    }

    int main(int argc, char *argv[]) { // Main function to start the server
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
            return 0;
        }
        int port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cout << "Invalid port number. Please provide a port between 1 and 65535." << std::endl;
            return 0;
        }
        try {
            run_server(port);
        } catch (const std::runtime_error &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        return 1;
    }