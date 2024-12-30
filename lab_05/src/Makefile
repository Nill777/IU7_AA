TARGET = lab_05
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -lpqxx -lpq
SRCS = main.cpp task.cpp database.cpp
HEADERS = task.hpp database.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean