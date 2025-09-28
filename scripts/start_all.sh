#!/bin/bash

# Chat Server Startup Script
# Starts both the C++ chat server and Python dashboard

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SERVER_DIR="../server"
DASHBOARD_DIR="../dashboard"
SERVER_PORT=8080
DASHBOARD_PORT=5001

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_dependencies() {
    print_status "Checking dependencies..."
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null; then
        print_error "g++ compiler not found. Please install g++"
        exit 1
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        print_error "Python 3 not found. Please install Python 3"
        exit 1
    fi
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_error "make not found. Please install make"
        exit 1
    fi
}

build_server() {
    print_status "Building C++ chat server..."
    
    cd "$SERVER_DIR"
    
    if make; then
        print_success "Server built successfully"
    else
        print_error "Failed to build server"
        exit 1
    fi
    
    cd - > /dev/null
}

setup_dashboard() {
    print_status "Setting up Python dashboard..."
    
    cd "$DASHBOARD_DIR"
    
    # Check if virtual environment exists
    if [ ! -d "venv" ]; then
        print_status "Creating virtual environment..."
        python3 -m venv venv
    fi
    
    # Activate virtual environment
    print_status "Activating virtual environment..."
    source venv/bin/activate
    
    # Install requirements
    print_status "Installing Python dependencies..."
    pip install -r requirements.txt
    
    cd - > /dev/null
}

check_port_available() {
    local port=$1
    local service=$2
    
    if lsof -Pi :$port -sTCP:LISTEN -t >/dev/null ; then
        print_error "Port $port is already in use by another program"
        print_status "You can:"
        print_status "1. Use a different port (currently configured: $port)"
        print_status "2. Identify and stop the program using: 'lsof -i :$port'"
        print_status "3. On macOS, disable AirPlay Receiver in System Preferences"
        return 1
    fi
    return 0
}

start_services() {
    print_status "Starting services..."

    # Check if ports are available
    if ! check_port_available $SERVER_PORT "chat server"; then
        exit 1
    fi
    
    if ! check_port_available $DASHBOARD_PORT "dashboard"; then
        print_warning "Dashboard port $DASHBOARD_PORT is in use, trying port 5001..."
        DASHBOARD_PORT=5001
        if ! check_port_available $DASHBOARD_PORT "dashboard"; then
            exit 1
        fi
    fi
    
    # Start the chat server in background
    print_status "Starting C++ chat server on port $SERVER_PORT..."
    cd "$SERVER_DIR"
    ./chat_server $SERVER_PORT &
    SERVER_PID=$!
    cd - > /dev/null
    
    # Wait a moment for server to start
    sleep 2
    
    # Check if server started successfully
    if ps -p $SERVER_PID > /dev/null; then
        print_success "Chat server started with PID $SERVER_PID"
    else
        print_error "Chat server failed to start"
        exit 1
    fi
    
    # Start the dashboard
    print_status "Starting Python dashboard on port $DASHBOARD_PORT..."
    cd "$DASHBOARD_DIR"
    source venv/bin/activate
    python app.py &
    DASHBOARD_PID=$!
    cd - > /dev/null
    
    # Wait a moment for dashboard to start
    sleep 3
    
    # Check if dashboard started successfully
    if ps -p $DASHBOARD_PID > /dev/null; then
        print_success "Dashboard started with PID $DASHBOARD_PID"
    else
        print_error "Dashboard failed to start"
        exit 1
    fi
    
    # Print access information
    echo
    print_success "All services started successfully!"
    echo
    echo -e "${GREEN}Access URLs:${NC}"
    echo -e "  Chat Server: ${YELLOW}Use './chat_client' to connect${NC}"
    echo -e "  Dashboard:   ${YELLOW}http://localhost:$DASHBOARD_PORT${NC}"
    echo
    echo -e "${GREEN}Process IDs:${NC}"
    echo -e "  Chat Server: $SERVER_PID"
    echo -e "  Dashboard:   $DASHBOARD_PID"
    echo
    print_warning "Press Ctrl+C to stop all services"
    
    # Setup cleanup on script exit
    trap cleanup EXIT
    
    # Wait for user interrupt
    wait
}

cleanup() {
    print_status "Shutting down services..."
    
    # Kill the processes
    if [ ! -z "$SERVER_PID" ] && ps -p $SERVER_PID > /dev/null; then
        kill $SERVER_PID 2>/dev/null && print_success "Chat server stopped"
    fi
    
    if [ ! -z "$DASHBOARD_PID" ] && ps -p $DASHBOARD_PID > /dev/null; then
        kill $DASHBOARD_PID 2>/dev/null && print_success "Dashboard stopped"
    fi
    
    print_success "All services stopped"
}

main() {
    echo -e "${BLUE}"
    echo "=========================================="
    echo "    Multi-Thread Chat Server Launcher"
    echo "=========================================="
    echo -e "${NC}"
    
    # Get the directory where this script is located
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cd "$SCRIPT_DIR"
    
    check_dependencies
    build_server
    setup_dashboard
    start_services
}

# Run main function
main "$@"