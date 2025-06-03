
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hostname> <port>" << std::endl;
        return 1;
    }

    const char* hostname = argv[1];
    int port = std::atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number." << std::endl;
        return 1;
    }

    // Resolve hostname
    
    struct hostent* server = gethostbyname(hostname);
    if (!server) {
        std::cerr << "No such host: " << hostname << std::endl;
        return 1;
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to server. Enter commands:" << std::endl;

    while (true) {
        std::cout << "\nChoose atom to add:\n";
        std::cout << "1. Add Carbon\n";
        std::cout << "2. Add Hydrogen\n";
        std::cout << "3. Add Oxygen\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;

        int count = 0;
        std::string line;

        switch (choice) {
            case 1:
                std::cout << "Enter number of carbon atoms to add: ";
                std::cin >> count;
                line = "ADD CARBON " + std::to_string(count) + "\n";
                break;
            case 2:
                std::cout << "Enter number of hydrogen atoms to add: ";
                std::cin >> count;
                line = "ADD HYDROGEN " + std::to_string(count) + "\n";
                break;
            case 3:
                std::cout << "Enter number of oxygen atoms to add: ";
                std::cin >> count;
                line = "ADD OXYGEN " + std::to_string(count) + "\n";
                break;
            case 4:
                close(sockfd);
                return 0;
            default:
                std::cout << "Invalid choice." << std::endl;
                close(sockfd);
                return 0;
        }
        if (count < 0) {
            std::cout << "Count must be a positive integer." << std::endl;
            
        }
        else{
            ssize_t sent = send(sockfd, line.c_str(), line.size(), 0);
            if (sent < 0) {
                std::cerr << "Send failed" << std::endl;
                break;
            }
        }
    }

    close(sockfd);
    return 0;
}