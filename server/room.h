// Chat rooms, broadcast, add/remove clients

#ifndef ROOM_H
#define ROOM_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class ClientHandler;

class ChatRoom {
 public:
  ChatRoom(const std::string& name);

  void broadcastMessage(const std::string& message,
                        const std::string& sender = "");
  void addClient(std::shared_ptr<ClientHandler> client);
  void removeClient(std::shared_ptr<ClientHandler> client);
  std::string getName() const;
  std::vector<std::string> getClientList();

 private:
  std::string name_;
  std::vector<std::shared_ptr<ClientHandler>> clients_;
  std::mutex clients_mutex_;
};

class RoomManager {
 public:
  static RoomManager& getInstance();

  std::shared_ptr<ChatRoom> getRoom(const std::string& name);
  std::vector<std::string> getRoomList();
  void removeEmptyRooms();

 private:
  RoomManager() = default;
  std::map<std::string, std::shared_ptr<ChatRoom>> rooms_;
  std::mutex rooms_mutex_;
};

#endif