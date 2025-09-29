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
