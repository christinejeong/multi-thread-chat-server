// Per-client thread handler (login, message handling, commands)

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

class ChatRoom;

class ClientHandler : public std::enable_shared_from_this<ClientHandler> {
 public:
  ClientHandler(int client_socket, struct sockaddr_in client_addr);
  ~ClientHandler();

  void start();
  void stop();
  void sendMessage(const std::string& message);
  std::string getUsername() const;
  bool isConnected() const;

 private:
  void handleClient();
  bool processCommand(const std::string& message);
  void sendRoomList();
  void sendClientList();

  int client_socket_;
  struct sockaddr_in client_addr_;
  std::string username_;
  std::shared_ptr<ChatRoom> current_room_;
  std::thread client_thread_;
  std::atomic<bool> connected_;
  std::atomic<bool> running_;
};

#endif