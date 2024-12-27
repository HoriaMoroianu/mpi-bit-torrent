CXX = mpic++

CXXFLAGS = -Wall -Wextra -pthread

TARGET = tema2

SRCS = main.cpp peer.cpp tracker.cpp

OBJS = $(SRCS:.cpp=.o)

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	mpirun -np 3 ./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
