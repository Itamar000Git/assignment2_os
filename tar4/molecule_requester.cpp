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
    // if (argc != 3) {
    //     std::cerr << "Usage: " << argv[0] << " <hostname> <port>" << std::endl;
    //     return 1;
    // }

    // const char* hostname = argv[1];
    // int port = std::atoi(argv[2]);
    
      if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << "-h <hostname> -p <port>" << std::endl;
        return 1;
    }
    const char* hostname;
    int port;
    int opt;
    bool has_host = false, has_port = false;
     while ((opt = getopt(argc, argv, "h:p:")) != -1) {
        switch (opt) {
             if(std::atoi(optarg) <= 0) {
                    std::cerr << "Error: any flag must be a positive integer." << std::endl;
                    exit(EXIT_FAILURE);
                }
            case 'h':
               
                hostname = optarg;
                has_host = true;
                break;
            case 'p':
            
                port = std::atoi(optarg);
                has_port = true;
                break;
           
            default:
                std::cerr << "Usage: " << argv[0]
                          << " -h <hostname> -p <port>\n";
                exit(EXIT_FAILURE);
        }
    }

    if (!has_host || !has_port) {
        std::cerr << "Error: Both -h (hostname) and -p (port) flags are required.\n";
        std::cerr << "Usage: " << argv[0]
                          << " -h <hostname> -p <port>\n";
        exit(EXIT_FAILURE);
    }


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
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    
    std::cout << "Connected to server over UDP. Enter commands:" << std::endl;
    int water_count = 0;
    int carbon_dioxide_count = 0;
    int alcohol_count = 0;
    int glucose_count = 0;

    while(true){
    std::cout << "What molecule do you want to create?";
    std::cout <<"\n1 - WATER \n2 - CARBON_DIOXIDE \n3 - ALCOHOL \n4 - GLUCOSE \n5 - EXIT"<<std::endl;
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1: // WATER
            std::cout << "Enter number of water molecules to create: ";
            std::cin >> water_count;
            break;
        case 2: // CARBON_DIOXIDE
            std::cout << "Enter number of carbon dioxide molecules to create: ";
            std::cin >> carbon_dioxide_count;
            break;
        case 3: // ALCOHOL
            std::cout << "Enter number of alcohol molecules to create: ";
            std::cin >> alcohol_count;
            break;
        case 4: // GLUCOSE
            std::cout << "Enter number of glucose molecules to create: ";
            std::cin >> glucose_count;
            break;
        default:
            std::cerr << "Invalid choice" << std::endl;
            close(sockfd);
            return 1;
    }
    std::string line;
    switch (choice) {
        case 1: // WATER
            line = "DELIVER WATER " + std::to_string(water_count) + "\n";
            break;
        case 2: // CARBON_DIOXIDE
            line = "DELIVER CARBON_DIOXIDE " + std::to_string(carbon_dioxide_count) + "\n";
            break;
        case 3: // ALCOHOL
            line = "DELIVER ALCOHOL " + std::to_string(alcohol_count) + "\n";
            break;
        case 4: // GLUCOSE
            line = "DELIVER GLUCOSE " + std::to_string(glucose_count) + "\n";
            break;
    }
    ssize_t sent = sendto(sockfd, line.c_str(), line.size(), 0,
                          (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (sent < 0) {
        std::cerr << "Send failed" << std::endl;
        close(sockfd);
        return 1;
    }

    char buffer[1024] = {0};
    socklen_t serv_addr_len = sizeof(serv_addr);
    ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                (struct sockaddr*)&serv_addr, &serv_addr_len);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        std::cout << "Server response: " << buffer;
    } else {
        std::cerr << "No response from server." << std::endl;
    }

    std::cout << "Do you want to deliver more? (y/press eny key to end) "<< std::endl;
    char more;
    std::cin >> more;
    if (more != 'y' && more != 'Y') {
        break; // Exit the loop if the user does not want to continue
    }

    
    }
    close(sockfd);

    return 0;
}