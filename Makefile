client: client.cpp
	g++ client.cpp -std=c++11 -o client
	./client ServerInfo.txt

server: server.cpp
	g++ server.cpp -std=c++11 -o server
	./server ServerInfo.txt

clean:
	rm client
	rm server

runClient:
	./client ServerInfo.txt

runServer:
	./server ServerInfo.txt
