all: server
	./server 8080

1: server
	./server 8081

2: server
	./server 8082

server: main.c
	clang -std=c23 -Ofast main.c -o server
