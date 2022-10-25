CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g 

TARGET = server
OBJS =  ./conn/*.cpp ./server/*.cpp \
       ./buffer/*.cpp ./queue.cpp ./main.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ./bin/$(TARGET)  -pthread 

clean:
	rm -rf ./bin/$(OBJS) $(TARGET)




