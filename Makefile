CFLAGS = -Wall -g

PORT = 1030

IP_SERVER = 127.0.0.1

all: server subscriber


server: server.cpp
	g++ server.cpp -o server -Wall


subscriber: subscriber.cpp
	g++ subscriber.cpp -o subscriber -Wall

.PHONY: clean run_server run_subscriber


run_server:
	./server ${PORT}

run_subscriber:
	./subscriber ${IP_SERVER} ${PORT}

clean:
	rm -f server subscriber
