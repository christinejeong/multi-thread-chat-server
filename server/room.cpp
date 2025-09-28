#include "room.h"
#include <algorithm>
#include <iostream>
#include "client_handler.h"

ChatRoom::ChatRoom(const std::string& name) : name_(name) {}

void ChatRoom::broadcastMessage(const std::string& message,
                                const std::string& sender) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  std::string formatted_message = message;
  if (!sender.empty()) { formatted_message = "[" + sender + "]: " + message; }

  // Remove disconnected clients while broadcasting
  clients_.erase(std::remove_if(clients_.begin(), clients_.end(),
                                [&](std::shared_ptr<ClientHandler> client) {
                                  if (client->isConnected()) {
                                    client->sendMessage(formatted_message);
                                    return false;
                                  }
                                  return true;
                                }),
                 clients_.end());
}

void ChatRoom::addClient(std::shared_ptr<ClientHandler> client) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  clients_.push_back(client);
  broadcastMessage(client->getUsername() + " joined the room", "SERVER");
}

void ChatRoom::removeClient(std::shared_ptr<ClientHandler> client) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  auto it = std::find(clients_.begin(), clients_.end(), client);
  if (it != clients_.end()) {
    clients_.erase(it);
    broadcastMessage(client->getUsername() + " left the room", "SERVER");
  }
}

std::string ChatRoom::getName() const { return name_; }

std::vector<std::string> ChatRoom::getClientList() {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  std::vector<std::string> client_list;
  for (const auto& client : clients_) {
    if (client->isConnected()) { client_list.push_back(client->getUsername()); }
  }
  return client_list;
}

// RoomManager implementation
RoomManager& RoomManager::getInstance() {
  static RoomManager instance;
  return instance;
}

std::shared_ptr<ChatRoom> RoomManager::getRoom(const std::string& name) {
  std::lock_guard<std::mutex> lock(rooms_mutex_);

  auto it = rooms_.find(name);
  if (it != rooms_.end()) { return it->second; }

  // Create new room if it doesn't exist
  auto new_room = std::make_shared<ChatRoom>(name);
  rooms_[name] = new_room;
  std::cout << "Created new room: " << name << std::endl;
  return new_room;
}

std::vector<std::string> RoomManager::getRoomList() {
  std::lock_guard<std::mutex> lock(rooms_mutex_);
  std::vector<std::string> room_list;
  for (const auto& pair : rooms_) { room_list.push_back(pair.first); }
  return room_list;
}

void RoomManager::removeEmptyRooms() {
  std::lock_guard<std::mutex> lock(rooms_mutex_);
  for (auto it = rooms_.begin(); it != rooms_.end();) {
    if (it->second->getClientList().empty()) {
      std::cout << "Removing empty room: " << it->first << std::endl;
      it = rooms_.erase(it);
    } else {
      ++it;
    }
  }
}