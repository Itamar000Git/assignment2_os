#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>



int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [-h <hostname> -p <port> | -f <UDS datagram file path>]" << std::endl;
        return 1;
    }
    const char* hostname = nullptr;
    int port = 0;
    int opt;
    bool has_host = false, has_port = false, has_file = false;
    std::string uds_dgram_path;

    while ((opt = getopt(argc, argv, "h:p:f:")) != -1) {
        switch (opt) {
            case 'h':
                hostname = optarg;
                has_host = true;
                break;
            case 'p':
                port = std::atoi(optarg);
                has_port = true;
                if (port <= 0 || port > 65535) {
                    std::cerr << "Error: Port must be a positive integer between 1 and 65535." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                uds_dgram_path = optarg;
                has_file = true;
                if (uds_dgram_path.empty()) {
                    std::cerr << "Error: UDS datagram file path cannot be empty." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                std::cerr << "Usage: " << argv[0]
                          << " [-h <hostname> -p <port> | -f <UDS datagram file path>]\n";
                exit(EXIT_FAILURE);
        }
    }

    if ((!has_host || !has_port) && !has_file) {
        std::cerr << "Error: Both -h (hostname) and -p (port) flags are required.\n";
        std::cerr << "Usage: " << argv[0]
                  << " [-h <hostname> -p <port> | -f <UDS datagram file path>]\n";
        exit(EXIT_FAILURE);
    }
    if (has_file && (has_host || has_port)) {
        std::cerr << "Error: Cannot use both hostname/port and UDS datagram file path together.\n";
        std::cerr << "Usage: " << argv[0]
                  << " [-h <hostname> -p <port> | -f <UDS datagram file path>]\n";
        exit(EXIT_FAILURE);
    }

    int sockfd = -1;
    struct sockaddr_storage server_addr_storage;
    socklen_t server_addr_len = 0;
    bool is_unix = false;
    std::string client_path;

    if (has_file) {
        // UDS datagram
        sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error opening UDS datagram socket" << std::endl;
            return 1;
        }
        // כתובת שרת
        struct sockaddr_un* serv_addr = (struct sockaddr_un*)&server_addr_storage;
        memset(serv_addr, 0, sizeof(*serv_addr));
        serv_addr->sun_family = AF_UNIX;
        strncpy(serv_addr->sun_path, uds_dgram_path.c_str(), sizeof(serv_addr->sun_path) - 1);
        server_addr_len = sizeof(*serv_addr);
        is_unix = true;

        // כתובת לקוח ייחודית
        client_path = "/tmp/molecule_req_" + std::to_string(getpid()) + ".sock";
        struct sockaddr_un client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sun_family = AF_UNIX;
        strncpy(client_addr.sun_path, client_path.c_str(), sizeof(client_addr.sun_path) - 1);
        unlink(client_path.c_str());
        if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
            std::cerr << "Error binding client UDS datagram socket" << std::endl;
            close(sockfd);
            return 1;
        }

        std::cout << "Connected to UDS datagram server. Enter commands:" << std::endl;
    } else {
        // UDP רגיל
        struct hostent* server = gethostbyname(hostname);
        if (!server) {
            std::cerr << "No such host: " << hostname << std::endl;
            return 1;
        }
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error opening socket" << std::endl;
            return 1;
        }
        struct sockaddr_in* serv_addr = (struct sockaddr_in*)&server_addr_storage;
        memset(serv_addr, 0, sizeof(*serv_addr));
        serv_addr->sin_family = AF_INET;
        memcpy(&serv_addr->sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr->sin_port = htons(port);
        server_addr_len = sizeof(*serv_addr);

        std::cout << "Connected to server over UDP. Enter commands:" << std::endl;
    }

    while (true) {
        std::cout << "What molecule do you want to create?";
        std::cout << "\n1 - WATER \n2 - CARBON_DIOXIDE \n3 - ALCOHOL \n4 - GLUCOSE \n5 - EXIT" << std::endl;
        int choice;
        std::cin >> choice;
        int count = 0;
        std::string line;
        switch (choice) {
            case 1:
                std::cout << "Enter number of water molecules to create: ";
                std::cin >> count;
                line = "DELIVER WATER " + std::to_string(count) + "\n";
                break;
            case 2:
                std::cout << "Enter number of carbon dioxide molecules to create: ";
                std::cin >> count;
                line = "DELIVER CARBON_DIOXIDE " + std::to_string(count) + "\n";
                break;
            case 3:
                std::cout << "Enter number of alcohol molecules to create: ";
                std::cin >> count;
                line = "DELIVER ALCOHOL " + std::to_string(count) + "\n";
                break;
            case 4:
                std::cout << "Enter number of glucose molecules to create: ";
                std::cin >> count;
                line = "DELIVER GLUCOSE " + std::to_string(count) + "\n";
                break;
            case 5:
                close(sockfd);
                if (is_unix) unlink(client_path.c_str());
                return 0;
            default:
                std::cerr << "Invalid choice" << std::endl;
                continue;
        }

        ssize_t sent = sendto(sockfd, line.c_str(), line.size(), 0,
                              (struct sockaddr*)&server_addr_storage, server_addr_len);
        if (sent < 0) {
            std::cerr << "Send failed" << std::endl;
            close(sockfd);
            if (is_unix) unlink(client_path.c_str());
            return 1;
        }

        char buffer[1024] = {0};
        // קבלת תשובה מהשרת
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            std::cout << "Server response: " << buffer;
        } else {
            std::cerr << "No response from server." << std::endl;
        }

        std::cout << "Do you want to deliver more? (y/press any key to end) " << std::endl;
        char more;
        std::cin >> more;
        if (more != 'y' && more != 'Y') {
            break;
        }
    }
    // close(sockfd);
    // if (is_unix) unlink(client_path.c_str());
    return 0;
}