// Runs the main server loop, accepts connections

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "client_handler.h"
#include "room.h"

class ChatServer {
 public:
  ChatServer(int port) : port_(port), running_(false) {}

  ~ChatServer() { stop(); }

  bool start() {
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == -1) {
      std::cerr << "Failed to create socket" << std::endl;
      return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt,
                   sizeof(opt)) < 0) {
      std::cerr << "Failed to set socket options" << std::endl;
      close(server_socket_);
      return false;
    }

    // Bind socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*) &server_addr,
             sizeof(server_addr)) < 0) {
      std::cerr << "Failed to bind socket to port " << port_ << std::endl;
      close(server_socket_);
      return false;
    }

    // Listen for connections
    if (listen(server_socket_, 10) < 0) {
      std::cerr << "Failed to listen on socket" << std::endl;
      close(server_socket_);
      return false;
    }

    std::cout << "Chat server started on port " << port_ << std::endl;
    std::cout << "Waiting for connections..." << std::endl;

    running_ = true;
    acceptThread_ = std::thread(&ChatServer::acceptConnections, this);
    cleanupThread_ = std::thread(&ChatServer::cleanupTask, this);

    return true;
  }

  void stop() {
    running_ = false;

    if (server_socket_ != -1) {
      close(server_socket_);
      server_socket_ = -1;
    }

    if (acceptThread_.joinable()) { acceptThread_.join(); }

    if (cleanupThread_.joinable()) { cleanupThread_.join(); }

    std::cout << "Chat server stopped" << std::endl;
  }

 private:
  void acceptConnections() {
    while (running_) {
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);

      int client_socket =
          accept(server_socket_, (struct sockaddr*) &client_addr, &client_len);

      if (client_socket < 0) {
        if (running_) {
          std::cerr << "Failed to accept client connection" << std::endl;
        }
        continue;
      }

      if (!running_) {
        close(client_socket);
        break;
      }

      // Create client handler and start it
      auto client_handler =
          std::make_shared<ClientHandler>(client_socket, client_addr);
      {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.push_back(client_handler);
      }
      client_handler->start();
    }
  }

  void cleanupTask() {
    while (running_) {
      std::this_thread::sleep_for(
          std::chrono::seconds(30));  // Cleanup every 30 seconds

      // Remove disconnected clients
      {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.erase(
            std::remove_if(clients_.begin(), clients_.end(),
                           [](std::shared_ptr<ClientHandler> client) {
                             return !client->isConnected();
                           }),
            clients_.end());
      }

      // Remove empty rooms
      RoomManager::getInstance().removeEmptyRooms();

      std::cout << "Cleanup completed. Active clients: " << clients_.size()
                << std::endl;
    }
  }

  int port_;
  int server_socket_{-1};
  std::atomic<bool> running_;
  std::thread acceptThread_;
  std::thread cleanupThread_;
  std::vector<std::shared_ptr<ClientHandler>> clients_;
  std::mutex clients_mutex_;
};

std::atomic<bool> g_running{true};

void signalHandler(int signal) {
  std::cout << "\nReceived signal " << signal << ", shutting down..."
            << std::endl;
  g_running = false;
}

int main(int argc, char* argv[]) {
  int port = 8080;

  if (argc > 1) { port = std::atoi(argv[1]); }

  // Setup signal handlers
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  ChatServer server(port);

  if (!server.start()) {
    std::cerr << "Failed to start server" << std::endl;
    return 1;
  }

  std::cout << "Server running. Press Ctrl+C to stop." << std::endl;

  // Main thread waits for shutdown signal
  while (g_running) { std::this_thread::sleep_for(std::chrono::seconds(1)); }

  server.stop();

  return 0;
}