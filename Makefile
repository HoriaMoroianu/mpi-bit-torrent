CXX = mpic++

CXXFLAGS = -Wall -pthread

TARGET = tema2

SRCS = main.cpp client.cpp tracker.cpp

OBJS = $(SRCS:.cpp=.o)

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
