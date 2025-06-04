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
#include <unistd.h>
#include <getopt.h>
#include <sys/un.h>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


int server_fd = -1, udp_fd = -1;
/**
 * @brief Signal handler for timeout. Closes the server if no activity for a specified time.
 * @param signum The signal number.
 * This function is called when the timeout signal is received.
 */
void timeout_handler(int signum) {
    std::cout << "No activity for "<<timeout<<" seconds. Closing server." << std::endl;
    close(server_fd);
    close(udp_fd);
    exit(0);
}
/// Function to handle cleanup on exit
void cleanup(int signum) {
if (server_fd != -1) close(server_fd);
if (udp_fd != -1) close(udp_fd);
std::cout << "\nSockets closed. Exiting." << std::endl;
exit(0);
}
/**
 * @brief prints the current stock of atoms.
 */
void printStock() {

    std::cout << "=== Current Stock ===" << std::endl;
    std::cout << "Total atoms   : " << my_stock->atom_count << std::endl;
    std::cout << "Carbon (C)    : " << my_stock->carbon_count << std::endl;
    std::cout << "Hydrogen (H)  : " << my_stock->hydrogen_count << std::endl;
    std::cout << "Oxygen (O)    : " << my_stock->oxygen_count << std::endl;
    std::cout << "=====================" << std::endl;
}
/**
 * @brief Checks if a string represents a valid integer.
 */
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

/**
 * @brief Checkes if there are enough atoms to deliver the specified number of water molecules.
 */
bool deliver_water(long long count){
    
    loadFromFile(save_file);
    if (my_stock->hydrogen_count < 2 * count || my_stock->oxygen_count < count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " water molecules." << std::endl;
        return false;
    }
    
    my_stock->hydrogen_count -= 2 * count;
    my_stock->oxygen_count -= count;
    my_stock->atom_count -= 3 * count; // 2H + O = H2O
    std::cout << "Delivered " << count << " water molecules." << std::endl;
    // printStock();
    update_file(save_file);
    return true;
}

/**
 * @brief Checkes if there are enough atoms to deliver the specified number of carbon dioxide molecules.
 */
bool deliver_carbon_dioxide(long long count) {

    loadFromFile(save_file);
    if (my_stock->carbon_count < count || my_stock->oxygen_count < 2 * count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " carbon dioxide molecules." << std::endl;
        return false;
    }
    

    my_stock->carbon_count -= count;
    my_stock->oxygen_count -= 2 * count;
    my_stock->atom_count -= 3 * count; //CO2
    std::cout << "Delivered " << count << " carbon dioxide molecules." << std::endl;
    //printStock();
    update_file(save_file);

    return true;
}

/**
 * @brief Checkes if there are enough atoms to deliver the specified number of alcohol molecules.
 */
bool deliver_alcohol(long long count) {

    loadFromFile(save_file);
    if (my_stock->carbon_count < 2*count || my_stock->hydrogen_count <  6*count || my_stock->oxygen_count < count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " alcohol molecules." << std::endl;
        return false;
    }
    

    my_stock->carbon_count -= 2*count;
    my_stock->hydrogen_count -= 6 * count;
    my_stock->oxygen_count -= count;
    my_stock->atom_count -= 9 * count; //C2H6O
    std::cout << "Delivered " << count << " alcohol molecules." << std::endl;
    //printStock();
    update_file(save_file);
    return true;
}

/**
 * @brief Checkes if there are enough atoms to deliver the specified number of glucose molecules.
 */
bool deliver_glucose(long long count) {
   
    loadFromFile(save_file);
    if (my_stock->carbon_count < 6*count || my_stock->hydrogen_count < 12 * count || my_stock->oxygen_count < 6*count) {
        std::cout << "Error: Not enough atoms to deliver " << count << " glucose molecules." << std::endl;
        return false;
    }
    

    my_stock->carbon_count -= 6*count;
    my_stock->hydrogen_count -= 12*count;
    my_stock->oxygen_count -= 6*count;
    my_stock->atom_count -= 24 * count; //C6H12O6
    std::cout << "Delivered " << count << " glucose molecules." << std::endl;
    //printStock();
    update_file(save_file);
    return true;
}
    
/**
 * @brief Adds carbon atoms to the stock and updates the file.
 */
void add_carbon(long long count) {
    loadFromFile(save_file);
    my_stock->carbon_count += count;
    my_stock->atom_count += count;
   
    update_file(save_file);
}

/**
 * @brief Adds hydrogen atoms to the stock and updates the file.
 */
void add_hydrogen(long long count) {
    loadFromFile(save_file);
    my_stock->hydrogen_count += count;
    my_stock->atom_count += count;
    update_file(save_file);
}

/**
 * @brief Adds oxygen atoms to the stock and updates the file.
 */
void add_oxygen(long long count) {
    loadFromFile(save_file);
    my_stock->oxygen_count += count;
    my_stock->atom_count += count;
    update_file(save_file);
}

/**
 * @brief Calculates the number of soft drinks that can be created with the current stock.
 * A soft drink consists of 1 water, 1 carbon dioxide, and 1 glucose molecule.
 */
long long num_of_soft_drinks(){
    loadFromFile(save_file);
    long long tmp_car=my_stock->carbon_count;
    long long tmp_hyd=my_stock->hydrogen_count;
    long long tmp_oxy=my_stock->oxygen_count;
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

/**
 * @brief Calculates the number of vodka that can be created with the current stock.
 * A vodka consists of 1 water, 1 alcohol, and 1 glucose molecule.
 */
long long num_of_vodka(){
    loadFromFile(save_file);
    long long tmp_car=my_stock->carbon_count;
    long long tmp_hyd=my_stock->hydrogen_count;
    long long tmp_oxy=my_stock->oxygen_count;
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
    std::cout << "Number of vodka that can be created: " << count << std::endl;
    return count;
}

/**
 * @brief Calculates the number of champagne that can be created with the current stock.
 * A champagne consists of 1 water, 1 alcohol, and 1 carbon dioxide molecule.
 */
long long num_of_champagne(){
    loadFromFile(save_file);
    long long tmp_car=my_stock->carbon_count;
    long long tmp_hyd=my_stock->hydrogen_count;
    long long tmp_oxy=my_stock->oxygen_count;
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
    std::cout << "Number of champagne that can be created: " << count << std::endl;
    return count;
}

/**
 * @brief Runs the server that listens for TCP and UDP connections.
 * @param port_tcp The TCP port to listen on.
 * @param port_udp The UDP port to listen on.
 * This function sets up a TCP socket and a UDP socket, listens for incoming connections,
 * and handles commands from both TCP clients and UDP datagrams.
 */
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
    // Set socket options to allow reuse of the address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("setsockopt failed");
    }

    // Set up the address structure for TCP
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_tcp);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {// Bind the socket to the address
        throw std::runtime_error("bind failed");
    }
    if (listen(server_fd, 10) < 0) {// Listen for incoming connections
        throw std::runtime_error("listen failed");
    }
    std::cout << "Server listening over tcp on port " << port_tcp << std::endl;

    // Create a UDP socket
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        throw std::runtime_error("UDP socket failed");
    }
    //set socket struct for udp using
    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(port_udp);
    if (bind(udp_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        throw std::runtime_error("UDP bind failed");
    }
    std::cout << "Server listening over udp on port " << port_udp << std::endl;

    while (true) {// Main loop for handling connections and commands
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds); // Add TCP server socket to the set
        FD_SET(udp_fd, &readfds); // Add UDP socket to the set
        FD_SET(0, &readfds); // Add stdin to the set for reading
       
        max_sd = server_fd;
        if (udp_fd > max_sd) max_sd = udp_fd; // Update max_sd to the highest socket descriptor
        if (max_sd < 0) max_sd = 0;

        for (int i = 0; i < FD_SETSIZE; i++) { // Add all client sockets to the set
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }
        
        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);// Wait for activity on the sockets
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
                    client_socket[i] = new_socket; // Store the new socket in the client_socket array
                    break;
                }
            }
            std::cout << "New connection, socket fd: " << new_socket << std::endl;
        }
        // Handle input from stdin
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
            alarm(timeout);
        }
        // Handle UDP activity
            if (FD_ISSET(udp_fd, &readfds)) {
            
            char udp_buffer[1024] = {0};
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            // Receive data from the UDP socket
            ssize_t udp_len = recvfrom(udp_fd, udp_buffer, sizeof(udp_buffer) - 1, 0,
                                    (struct sockaddr*)&client_addr, &addrlen);
            if (udp_len > 0) {
                udp_buffer[udp_len] = '\0';
                std::cout << "Received UDP: " << udp_buffer << std::endl;
                std::istringstream iss(udp_buffer);
                std::string line;
                while (std::getline(iss, line)) { // Read each line from the UDP buffer
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
                        // Send response back to the client
                        ssize_t sent = sendto(udp_fd, response.c_str(), response.size(), 0,
                                            (struct sockaddr*)&client_addr, addrlen);
                        if (sent < 0) {
                            std::cerr << "Send failed" << std::endl;
                        }
                        printStock();
                        alarm(timeout);
                    }
                    
                }
            }
        }

        // Handle TCP client activity
        for (int i = 0; i < FD_SETSIZE; i++) {
            sd = client_socket[i]; // Check each client socket for activity
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buffer[1024] = {0};
                valread = read(sd, buffer, sizeof(buffer) - 1); // Read data from the client socket
                if (valread <= 0) {
                    close(sd);
                    client_socket[i] = 0;
                    std::cout << "Client disconnected, socket fd: " << sd << std::endl;
                } else {
                    buffer[valread] = '\0';
                    std::istringstream iss(buffer);
                    std::string line;
                    while (std::getline(iss, line)) { // Read each line from the client buffer
                        std::istringstream line_stream(line);
                        std::string cmd, atom, amount_str;
                        line_stream >> cmd >> atom >> amount_str;
                        if (cmd == "ADD" && isInteger(amount_str)) { // Check if the command is to add atoms
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
                            alarm(timeout);
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

/**
 * @brief Runs the server that listens for UDS (Unix Domain Socket) connections.
 * @param has_datagram_path Indicates if a UDS datagram path is provided.
 * @param has_stream_path Indicates if a UDS stream path is provided.
 * This function sets up a UDS stream socket and a UDS datagram socket, listens for incoming connections,
 * and handles commands from both UDS stream clients and UDS datagram messages.
 */
void run_server_uds(bool has_datagram_path, bool has_stream_path) {
    
    int uds_stream_fd = -1, uds_dgram_fd = -1, new_socket, max_sd, activity, valread, sd;
    int client_socket[FD_SETSIZE] = {0};
    fd_set readfds;
    // Create a uds stream
    if (!stream_path.empty()) {
        uds_stream_fd = socket(AF_UNIX, SOCK_STREAM, 0); // Create a UDS stream socket
        if (uds_stream_fd < 0) throw std::runtime_error("UDS dgram socket failed");

        // Set up the address structure for UDS stream
        struct sockaddr_un stream_addr;
        memset(&stream_addr, 0, sizeof(stream_addr));
        stream_addr.sun_family = AF_UNIX;
        strncpy(stream_addr.sun_path, stream_path.c_str(), sizeof(stream_addr.sun_path) - 1);

        unlink(stream_path.c_str()); 
        if (bind(uds_stream_fd, (struct sockaddr*)&stream_addr, sizeof(stream_addr)) < 0)
            throw std::runtime_error("UDS stream bind failed");
        if (listen(uds_stream_fd, 10) < 0)
            throw std::runtime_error("UDS stream listen failed");

        std::cout << "Server listening over UDS stream at " << stream_path << std::endl;
    }
    // Create a uds datagram
     if (!datagram_path.empty()) {
        uds_dgram_fd = socket(AF_UNIX, SOCK_DGRAM, 0); // Create a UDS datagram socket
        if (uds_dgram_fd < 0) throw std::runtime_error("UDS stream socket failed");

        struct sockaddr_un dgram_addr;
        memset(&dgram_addr, 0, sizeof(dgram_addr));
        dgram_addr.sun_family = AF_UNIX;
        strncpy(dgram_addr.sun_path, datagram_path.c_str(), sizeof(dgram_addr.sun_path) - 1);

        unlink(datagram_path.c_str()); 
        if (bind(uds_dgram_fd, (struct sockaddr*)&dgram_addr, sizeof(dgram_addr)) < 0)
            throw std::runtime_error("UDS datagram bind failed");
       

        std::cout << "Server listening over UDS datagram at " << datagram_path << std::endl;
    }
  
    while (true) {// Main loop for handling UDS connections and commands
        FD_ZERO(&readfds);
        if (uds_stream_fd != -1) FD_SET(uds_stream_fd, &readfds);
        if (uds_dgram_fd != -1) FD_SET(uds_dgram_fd, &readfds);
        FD_SET(0, &readfds); // Add stdin to the set for reading
       
        max_sd =0;
        // Update max_sd to the highest socket descriptor
        if (uds_stream_fd > max_sd) max_sd = uds_stream_fd;
        if (uds_dgram_fd > max_sd) max_sd = uds_dgram_fd;

        for (int i = 0; i < FD_SETSIZE; i++) { // Add all client sockets to the set
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr); // Wait for activity on the sockets
        if (activity < 0 && errno != EINTR) {
            std::cout << "select error" << std::endl;
            break;
        }
        // Handle input from stdin
        if (FD_ISSET(0, &readfds)) {
        std::string input;
        std::getline(std::cin, input); // Read input from stdin
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
            alarm(timeout);
        }
        // Handle new UDS stream connections
        if (uds_stream_fd != -1 && FD_ISSET(uds_stream_fd, &readfds)) {
            struct sockaddr_un client_addr;
            socklen_t addrlen = sizeof(client_addr);
            new_socket = accept(uds_stream_fd, (struct sockaddr*)&client_addr, &addrlen);// Accept a new UDS stream connection
            if (new_socket < 0) {
                std::cout << "accept error" << std::endl;
                continue;
            }
            for (int i = 0; i < FD_SETSIZE; i++) {// Find an empty slot in the client_socket array
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
            std::cout << "New UDS stream connection, socket fd: " << new_socket << std::endl;
        }
        // Handle new UDS datagram connections
            if (uds_dgram_fd!=-1&&FD_ISSET(uds_dgram_fd, &readfds)) {
            char dgram_buffer[1024] = {0};
            struct sockaddr_un client_addr;
            socklen_t addrlen = sizeof(client_addr);
            // Receive data from the UDS datagram socket
            ssize_t dgram_len = recvfrom(uds_dgram_fd, dgram_buffer, sizeof(dgram_buffer) - 1, 0,
                                    (struct sockaddr*)&client_addr, &addrlen);
            if (dgram_len > 0) {
                dgram_buffer[dgram_len] = '\0';
                std::cout << "Received UDS datagram: " << dgram_buffer << std::endl;
                std::istringstream iss(dgram_buffer);
                std::string line;
                while (std::getline(iss, line)) { // Read each line from the UDS datagram buffer
                    std::istringstream line_stream(line);
                    std::string cmd, molecule, amount_str;
                    line_stream >> cmd >> molecule >> amount_str;
                    if (cmd == "DELIVER" && isInteger(amount_str)) {
                        int amount = std::stoi(amount_str);
                        std::string response;
                        if (molecule == "WATER") { // Check if the command is to deliver water
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

                        ssize_t sent = sendto(uds_dgram_fd, response.c_str(), response.size(), 0,
                                            (struct sockaddr*)&client_addr, addrlen);
                        if (sent < 0) {
                            std::cerr << "Send failed" << std::endl;
                        }
                        printStock();
                        alarm(timeout);
                    } 
                   
                }
            }
        }

        // Handle uds stream client activity
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
                    while (std::getline(iss, line)) { // Read each line from the client buffer
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
                            alarm(timeout);
                        } else if (!cmd.empty()) {
                            std::cout << "Error: Invalid command: " << line << std::endl;
                        }
                    }
                }
            }
        }
    }
    // Cleanup: close sockets and unlink UDS paths
    if (uds_stream_fd != -1) close(uds_stream_fd);
    if (uds_dgram_fd != -1) close(uds_dgram_fd);
    if (!stream_path.empty()) unlink(stream_path.c_str());
    if (!datagram_path.empty()) unlink(datagram_path.c_str());
}

/**
 * @brief Updates the stock of atoms in the file.
 * @param save_file The file where the stock is saved.
 */
void updateStock(int argc, char* argv[], int& port_tcp, int& port_udp) {
    int opt;
    bool has_tcp = false, has_udp = false;
    
    while ((opt = getopt_long(argc, argv, "o:c:h:t:T:U:s:d:f:", long_options, nullptr)) != -1) {// Parse command line options
        switch (opt) {
             if(std::atoi(optarg) <= 0) {
                    std::cerr << "Error: any flag must be a positive integer." << std::endl;
                    exit(EXIT_FAILURE);
                }
            case 'o':
                add_oxygen(std::atoi(optarg));
                break;
            case 'c':
                add_carbon(std::atoi(optarg));
                break;
            case 'h':
                add_hydrogen(std::atoi(optarg));
                break;
            case 't':
                timeout = std::atoi(optarg);
                break;
            case 'T':
                port_tcp = std::atoi(optarg);
                has_tcp = true;
                if (port_tcp <= 0|| port_tcp > 65535) { // Check if TCP port is a valid positive integer
                    std::cerr << "Error: TCP port must be a positive integer." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'U':
                port_udp = std::atoi(optarg);
                has_udp = true;
                if (port_udp <= 0 || port_udp > 65535) {
                    std::cerr << "Error: UDP port must be a positive integer." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                stream_path = optarg;
                has_stream_path = true;
                break;
            case 'd':
                datagram_path = optarg;
                has_datagram_path = true;
                break;
            case 'f':
                save_file = optarg;
                load_from_file= true;
                struct stat st;
                if (stat(save_file.c_str(), &st) != 0) {
                    new_file = true;
                    int fd = open(save_file.c_str(), O_RDWR | O_CREAT , 0666); // Open the file for reading and writing
                    if (fd < 0) {
                        std::cerr << "Error: Could not open filee " << save_file << std::endl;
                        return;
                    }
                }
                break;
            default:
                std::cerr << "Usage: " << argv[0]
                          << " -T <tcp_port> -U <udp_port> [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
                exit(EXIT_FAILURE);
        }
    }
    // Check if ther is ambiguous options
      if ((has_tcp || has_udp) && (has_stream_path || has_datagram_path)) {
        std::cerr << "Error: Cannot use both TCP/UDP and UDS options together.\n";
        std::cerr << "Usage: " << argv[0]
                  << " [-T <tcp_port> -U <udp_port> | -s <UDS stream file path> -d <UDS datagram file path>] [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
        exit(EXIT_FAILURE);
    }
    if ((has_tcp || has_udp) && (!has_tcp || !has_udp)) {
        std::cerr << "Error: Both -T (TCP port) and -U (UDP port) flags are required.\n";
          std::cerr << "Usage: " << argv[0]
                  << " [-T <tcp_port> -U <udp_port> | -s <UDS stream file path> -d <UDS datagram file path>] [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
        exit(EXIT_FAILURE);
    }
    if ((has_stream_path || has_datagram_path) && (!has_stream_path || !has_datagram_path)) {
        std::cerr << "Error: Both -s (stream path) and -d (datagram path) flags are required for UDS mode.\n";
        std::cerr << "Usage: " << argv[0]
                  << " [-T <tcp_port> -U <udp_port> | -s <UDS stream file path> -d <UDS datagram file path>] [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
        exit(EXIT_FAILURE);
    }
    if (!((has_tcp && has_udp) || (has_stream_path && has_datagram_path))) {
        std::cerr << "Error: Must provide either both TCP/UDP or both UDS stream/datagram options.\n";
        std::cerr << "Usage: " << argv[0]
                  << " [-T <tcp_port> -U <udp_port> | -s <UDS stream file path> -d <UDS datagram file path>] [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
        exit(EXIT_FAILURE);
    }
    // Initialize the stock
    if(new_file==true){
            update_file(save_file);
            new_file = false;
    }
     
}

/**
 * @brief Loads the stock from a file.
 * @param save_file The file from which to load the stock.
 * This function maps the file to memory, reads the stock data, and updates the in-memory stock structure.
 * It also applies a write lock to ensure exclusive access while reading the file.
 * If the file does not exist, it creates a new file with the initial stock values.
 */
void loadFromFile(std::string save_file){   
if(load_from_file==true){
   if(new_file==true){
            update_file(save_file);
            new_file = false;
    }
    int fd = open(save_file.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        std::cerr << "Error: Could not open file " << save_file << std::endl;
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, sizeof(Stock)) == -1) { // Set the file size to the size of Stock
        std::cerr << "Error: Could not set file size" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }
    struct flock fl; // File lock structure
    fl.l_type = F_WRLCK; // Set the lock type to write lock
    fl.l_whence = SEEK_SET; // Set the lock starting point to the beginning of the file
    fl.l_start = 0; // Start locking from the beginning of the file
    fl.l_len = 0; // Lock the entire file
    fcntl(fd, F_SETLKW, &fl); // Apply the lock
    // Map the file to memory
    Stock* mapped_stock = (Stock*)mmap(NULL, sizeof(Stock), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_stock == MAP_FAILED) {
        std::cerr << "Error: mmap failed" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Initialize the stock from the mapped memory
    my_stock->atom_count = mapped_stock->atom_count;
    my_stock->carbon_count = mapped_stock->carbon_count;
    my_stock->hydrogen_count = mapped_stock->hydrogen_count;
    my_stock->oxygen_count = mapped_stock->oxygen_count;

    fl.l_type = F_UNLCK; // Set the lock type to unlock
    fcntl(fd, F_SETLK, &fl); // Release the lock
    munmap(mapped_stock, sizeof(Stock)); // Unmap the memory
    close(fd);
}
}

/**
 * @brief Updates the stock in the file with the current values.
 * @param file_p The file where the stock is saved.
 * This function maps the file to memory, updates the stock values, and synchronizes the changes to the file.
 * It also applies a write lock to ensure exclusive access while writing to the file.
 */
void update_file(std::string file_p) {

    if(load_from_file==true){
        
       // loadFromFile(file_p);
        int fd = open(file_p.c_str(), O_RDWR | O_CREAT , 0666); // Open the file for reading and writing
        if (fd < 0) {
            std::cerr << "Error: Could not open filee " << file_p << std::endl;
            return;
        }
        
        if (ftruncate(fd, sizeof(Stock)) == -1) {
        std::cerr << "Error: Could not set file size" << std::endl;
        close(fd);
        return;
        }
        struct flock fl; // File lock structure
        fl.l_type = F_WRLCK; // Set the lock type to write lock
        fl.l_whence = SEEK_SET; // Set the lock starting point to the beginning of the file
        fl.l_start = 0; // Start locking from the beginning of the file
        fl.l_len = 0; // Lock the entire file
        fcntl(fd, F_SETLKW, &fl); // Apply the lock
        // Map the file to memory
        Stock* mapped_stock = (Stock*)mmap(NULL, sizeof(Stock), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mapped_stock == MAP_FAILED) {
            std::cerr << "Error: mmap failed" << std::endl;
            close(fd);
            return;
        }

        // Update the stock values in the mapped memory
        mapped_stock->atom_count = my_stock->atom_count;
        mapped_stock->carbon_count = my_stock->carbon_count;
        mapped_stock->hydrogen_count = my_stock->hydrogen_count;
        mapped_stock->oxygen_count = my_stock->oxygen_count;
        msync(mapped_stock, sizeof(Stock), MS_SYNC); // Synchronize the changes to the file

        fl.l_type = F_UNLCK;// Set the lock type to unlock
        fcntl(fd, F_SETLK, &fl); // Release the lock
        munmap(mapped_stock, sizeof(Stock)); // Unmap the memory
        close(fd);
        //printStock();
    }
    
}



int main(int argc, char *argv[]) { // Main function to start the server
    if (argc < 3) { // Check if the required arguments are provided
          std::cerr << "Usage: " << argv[0]
                  << " [-T <tcp_port> -U <udp_port> | -s <UDS stream file path> -d <UDS datagram file path>] [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]\n";
        return 0;
    }


    int port_tcp ;
    int port_udp ;

   
    updateStock(argc, argv, port_tcp, port_udp);


    if(load_from_file==true && new_file==false){
       loadFromFile(save_file);
    }



    
    printStock();
    signal(SIGINT, cleanup);// Register signal handler for cleanup on Ctrl+C
    signal(SIGALRM, timeout_handler); // Register signal handler for timeout
    alarm(timeout); // Set the initial timeout

    

    try {
        if(has_datagram_path || has_stream_path) {
              run_server_uds(has_datagram_path, has_stream_path );  // Run the server with UDS sockets

        }
        else{
            run_server(port_tcp, port_udp); // Run the server with TCP and UDP sockets
        }
    } catch (const std::runtime_error &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    delete my_stock; // Clean up the stock object
    return 1;
}