CC = clang -c
CFLAGS = -O3 -std=c2y -pipe
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -fno-pic -fno-PIC -fno-pie -fno-plt
LD = clang -fuse-ld=mold
LDFLAGS = -no-pie -Wl,-X,-s,-S,--as-needed,--gc-sections,--icf=all

test = $(wildcard test/*)
objs = $(test:test/%.c=objs/%.o)
all: server
	./server 8080

1: server
	./server 8081

2: server
	./server 8082

objs/main.o: main.c head.html
	$(CC) $(CFLAGS) main.c -o objs/main.o

objs/%.o: test/%.c random 
	$(CC) $(CFLAGS) $< -o $@

ques: $(objs)


server: objs/main.o
	$(LD) $(LDFLAGS) objs/main.o -o server

clean: 
	rm objs/* server


