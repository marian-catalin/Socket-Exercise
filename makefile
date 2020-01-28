server: server.c
		gcc -g server.c -o server -lpthread

client: client.c
		gcc -g client.c -o client -lpthread
