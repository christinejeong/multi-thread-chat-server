#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

class ChatClient {
 public:
  ChatClient(const std::string& server_ip, int port)
      : server_ip_(server_ip), port_(port), connected_(false) {}

  ~ChatClient() { disconnect(); }

  bool connectToServer() {
    // Create socket
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
      std::cerr << "Failed to create socket" << std::endl;
      return false;
    }

    // Setup server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) {
      std::cerr << "Invalid server address" << std::endl;
      close(sockfd_);
      return false;
    }

    // Connect to server
    if (connect(sockfd_, (struct sockaddr*) &server_addr, sizeof(server_addr)) <
        0) {
      std::cerr << "Failed to connect to server" << std::endl;
      close(sockfd_);
      return false;
    }

    connected_ = true;
    std::cout << "Connected to chat server at " << server_ip_ << ":" << port_
              << std::endl;

    return true;
  }

  void run() {
    if (!connected_) {
      std::cerr << "Not connected to server" << std::endl;
      return;
    }

    // Start receiver thread
    std::thread receiver_thread(&ChatClient::receiveMessages, this);

    // Main thread handles user input
    std::string input;
    while (connected_ && std::getline(std::cin, input)) {
      if (!input.empty()) {
        if (!sendMessage(input)) {
          std::cerr << "Failed to send message" << std::endl;
          break;
        }

        if (input == "/quit") { break; }
      }
    }

    connected_ = false;
    if (receiver_thread.joinable()) { receiver_thread.join(); }
  }

 private:
  void receiveMessages() {
    char buffer[1024];

    while (connected_) {
      ssize_t bytes_received = recv(sockfd_, buffer, sizeof(buffer) - 1, 0);

      if (bytes_received <= 0) {
        std::cout << "\nDisconnected from server" << std::endl;
        connected_ = false;
        break;
      }

      buffer[bytes_received] = '\0';
      std::cout << buffer;
      std::cout.flush();
    }
  }

  bool sendMessage(const std::string& message) {
    std::string msg = message + "\n";
    ssize_t sent = send(sockfd_, msg.c_str(), msg.length(), 0);
    return sent > 0;
  }

  void disconnect() {
    connected_ = false;
    if (sockfd_ != -1) {
      close(sockfd_);
      sockfd_ = -1;
    }
  }

  std::string server_ip_;
  int port_;
  int sockfd_{-1};
  std::atomic<bool> connected_;
};

int main(int argc, char* argv[]) {
  std::string server_ip = "127.0.0.1";
  int port = 8080;

  if (argc >= 2) { server_ip = argv[1]; }
  if (argc >= 3) { port = std::atoi(argv[2]); }

  ChatClient client(server_ip, port);

  if (!client.connectToServer()) { return 1; }

  client.run();

  return 0;
}