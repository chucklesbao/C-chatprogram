client: client.cpp
	g++ client.cpp -std=c++11 -o client
	./client ServerInfo.txt

clean:
	rm client