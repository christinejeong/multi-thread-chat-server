#include "client_handler.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include "room.h"

#define BUFFER_SIZE 1024

ClientHandler::ClientHandler(int client_socket, struct sockaddr_in client_addr)
    : client_socket_(client_socket),
      client_addr_(client_addr),
      connected_(true),
      running_(true) {
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(client_addr_.sin_addr), client_ip, INET_ADDRSTRLEN);
  std::cout << "New client connected from " << client_ip << ":"
            << ntohs(client_addr_.sin_port) << std::endl;
}

ClientHandler::~ClientHandler() {
  stop();
  if (client_thread_.joinable()) { client_thread_.join(); }
}

void ClientHandler::start() {
  client_thread_ = std::thread(&ClientHandler::handleClient, this);
}

void ClientHandler::stop() {
  running_ = false;
  connected_ = false;
  if (client_socket_ != -1) {
    close(client_socket_);
    client_socket_ = -1;
  }
}

void ClientHandler::handleClient() {
  char buffer[BUFFER_SIZE];

  // Welcome message and username setup
  std::string welcome_msg =
      "Welcome to the Chat Server!\nEnter your username: ";
  send(client_socket_, welcome_msg.c_str(), welcome_msg.length(), 0);

  // Get username
  ssize_t bytes_received = recv(client_socket_, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_received <= 0) {
    std::cout << "Client disconnected during username setup" << std::endl;
    connected_ = false;
    return;
  }

  buffer[bytes_received] = '\0';
  username_ = std::string(buffer);
  username_.erase(std::remove(username_.begin(), username_.end(), '\n'),
                  username_.end());
  username_.erase(std::remove(username_.begin(), username_.end(), '\r'),
                  username_.end());

  if (username_.empty()) { username_ = "Anonymous"; }

  std::cout << "Client username set to: " << username_ << std::endl;

  // Join default room
  current_room_ = RoomManager::getInstance().getRoom("general");
  current_room_->addClient(shared_from_this());

  std::string join_msg =
      "Joined room: general\nType '/help' for available commands\n";
  sendMessage(join_msg);

  // Main message handling loop
  while (running_ && connected_) {
    bytes_received = recv(client_socket_, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0) {
      std::cout << "Client " << username_ << " disconnected" << std::endl;
      connected_ = false;
      break;
    }

    buffer[bytes_received] = '\0';
    std::string message(buffer);
    message.erase(std::remove(message.begin(), message.end(), '\n'),
                  message.end());
    message.erase(std::remove(message.begin(), message.end(), '\r'),
                  message.end());

    if (!message.empty()) {
      if (message[0] == '/') {
        // Process command
        if (!processCommand(message)) {
          sendMessage("Unknown command. Type /help for available commands.");
        }
      } else {
        // Broadcast message to room
        if (current_room_) {
          current_room_->broadcastMessage(message, username_);
        }
      }
    }
  }

  // Cleanup
  if (current_room_) { current_room_->removeClient(shared_from_this()); }
  connected_ = false;
}

bool ClientHandler::processCommand(const std::string& message) {
  std::istringstream iss(message);
  std::string command;
  iss >> command;

  if (command == "/help") {
    std::string help_msg =
        "Available commands:\n"
        "/help - Show this help message\n"
        "/rooms - List all available rooms\n"
        "/join <room> - Join a room\n"
        "/leave - Leave current room\n"
        "/list - List users in current room\n"
        "/quit - Disconnect from server\n";
    sendMessage(help_msg);
    return true;
  } else if (command == "/rooms") {
    sendRoomList();
    return true;
  } else if (command == "/join") {
    std::string room_name;
    iss >> room_name;
    if (!room_name.empty()) {
      auto new_room = RoomManager::getInstance().getRoom(room_name);
      if (current_room_) { current_room_->removeClient(shared_from_this()); }
      current_room_ = new_room;
      current_room_->addClient(shared_from_this());
      sendMessage("Joined room: " + room_name);
    } else {
      sendMessage("Usage: /join <room_name>");
    }
    return true;
  } else if (command == "/leave") {
    if (current_room_) {
      std::string room_name = current_room_->getName();
      current_room_->removeClient(shared_from_this());
      current_room_ = RoomManager::getInstance().getRoom("general");
      current_room_->addClient(shared_from_this());
      sendMessage("Left room " + room_name + " and joined general");
    }
    return true;
  } else if (command == "/list") {
    sendClientList();
    return true;
  } else if (command == "/quit") {
    sendMessage("Goodbye!");
    connected_ = false;
    return true;
  }

  return false;
}

void ClientHandler::sendRoomList() {
  auto rooms = RoomManager::getInstance().getRoomList();
  std::string room_list = "Available rooms:\n";
  for (const auto& room : rooms) { room_list += " - " + room + "\n"; }
  sendMessage(room_list);
}

void ClientHandler::sendClientList() {
  if (current_room_) {
    auto clients = current_room_->getClientList();
    std::string client_list =
        "Users in room '" + current_room_->getName() + "':\n";
    for (const auto& client : clients) { client_list += " - " + client + "\n"; }
    sendMessage(client_list);
  }
}

void ClientHandler::sendMessage(const std::string& message) {
  if (connected_ && client_socket_ != -1) {
    std::string formatted_msg = message + "\n";
    ssize_t sent =
        send(client_socket_, formatted_msg.c_str(), formatted_msg.length(), 0);
    if (sent <= 0) { connected_ = false; }
  }
}

std::string ClientHandler::getUsername() const { return username_; }

bool ClientHandler::isConnected() const { return connected_; }