SFLAGS = -fno-stack-protector

client: client.c
	gcc client.c -o client

server: server_socket.c database.c
	gcc server_socket.c database.c -l sqlite3 -o server