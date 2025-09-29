# Welcome to the Multi-threaded Chat Server!
A high-performance, multi-threaded chat application built with C++ for the backend server and Python/Flask for the real-time monitoring dashboard. This project demonstrates modern systems programming, concurrent network applications, and full-stack development.
## Features

### Core Chat Server (C++)
- **Multi-threaded Architecture**: Handles multiple concurrent clients efficiently using thread pooling
- **Room-based Messaging**: Create and join different chat rooms with isolated conversations
- **TCP Socket Programming**: Robust network communication with proper connection lifecycle management
- **Thread Safety**: Implements mutexes and smart pointers following RAII principles
- **Command System**: Built-in commands (`/join`, `/rooms`, `/list`, `/help`, `/quit`)

### Real-time Dashboard (Python/Flask)
- **Live Monitoring**: Real-time statistics on active clients, rooms, and server health
- **Responsive Design**: Clean, modern UI with custom CSS using a purple/white color scheme
- **Web Interface**: Beautiful dashboard accessible via web browser
- **Auto-updating**: Statistics refresh every 3 seconds without page reload

### Technical Highlights
- **Cross-language Integration**: C++ backend with Python frontend demonstrating polyglot programming
- **Object-Oriented Design**: Clean separation of concerns with Room, ClientHandler, and Server classes
- **Memory Management**: Smart pointers and RAII for automatic resource cleanup
- **Error Handling**: Comprehensive error handling and graceful degradation

## Technology Stack

| Component | Technology |
|-----------|------------|
| **Backend Server** | C++17, POSIX Sockets, Multi-threading |
| **Monitoring Dashboard** | Python 3, Flask, HTML5, CSS3, JavaScript |
| **Concurrency** | std::thread, std::mutex, std::lock_guard |
| **Networking** | TCP Sockets, BSD Socket API |
| **Build System** | GNU Make |
| **Platform** | Linux/macOS |


## Quick Start

### Prerequisites
- C++17 compatible compiler (g++ 7+ or clang++)
- Python 3.8+
- GNU Make

### Installation & Running

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/multi-thread-chat-server.git
   cd multi-thread-chat-server

2. **Start all services**
   ```bash
   ./scripts/start_all.sh

4. **Manual setup**
   # Build and run C++ server
  cd server && make && ./chat_server 8080

  # In another terminal - Start Python dashboard
  cd dashboard
  python3 -m venv venv
  source venv/bin/activate
  pip install -r requirements.txt
  python app.py --port 5001
  
  # In another terminal - Connect chat client
  cd client && make && ./chat_client 127.0.0.1 8080
