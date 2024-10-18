all: server
	./server 8080

1: server
	./server 8081

server: main.c
	clang -Ofast main.c -o server
