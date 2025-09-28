CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
SRCDIR = .
SOURCES = $(SRCDIR)/chat_server.cpp $(SRCDIR)/client_handler.cpp $(SRCDIR)/room.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = chat_server

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean