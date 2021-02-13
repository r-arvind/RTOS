default: client server
clean: clean-client client clean-server server
client:
	gcc src/client.c -I include -pthread -o bin/client
server:
	gcc src/server.c -I include -pthread -o bin/server
clean-client:
	rm bin/client
clean-server:
	rm bin/server