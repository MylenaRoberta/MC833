client: src/client_server.c src/client.c
	gcc src/client_server.c src/client.c -o bin/client

server: src/database.c src/client_server.c src/server.c
	gcc src/database.c src/client_server.c src/server.c -l sqlite3 -o bin/server