default: client server
clean: clean-client client clean-server server
client:
	gcc client.c -o client -pthread
server:
	gcc server.c -o server -pthread
clean-client:
	rm client
clean-server:
	rm server