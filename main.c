#define _GNU_SOURCE

#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const char header[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

static socklen_t SOCKLEN = sizeof(struct sockaddr);
#define BS 65536

static int server_socket;
static void sigint_handle(int sig) {
  close(server_socket);
  exit(0);
}

static inline void socket_init(int port) {
  if (!(server_socket = socket(AF_INET, SOCK_STREAM, 0)))
    exit(1);
  signal(SIGINT, sigint_handle);

  if (bind(server_socket,
           (struct sockaddr *)&(struct sockaddr_in){
               .sin_family = AF_INET,
               .sin_addr.s_addr = INADDR_ANY,
               .sin_port = htons(port),
           },
           SOCKLEN) < 0)
    exit(2);

  if (listen(server_socket, 3) < 0)
    exit(3);
}

static void *client_handle(void *arg) {
  int bs, len, pipefds[2], client_socket = *(int *)arg;
  pipe(pipefds);
  char buf[BS];

  bs = read(client_socket, buf, BS);
  buf[bs] = 0;
  puts(buf);

  switch (buf[0]) {
    char boundary[128], *pos, *end;
    int file_size, fd;
  case 'G':
    break;

  case 'P':
    pos = strstr(buf, "boundary=");
    sscanf(pos, "boundary=%s", boundary);
    len = strlen(boundary);
    pos += len;
    if (!strstr(pos, boundary)) {
      bs = read(client_socket, buf, BS);
      buf[bs] = 0;
      if (!(pos = strstr(buf, boundary)))
        ;
      if (!(pos = strstr(pos, "\r\n\r\n")))
        ;
      if (!(end = strstr(pos, boundary)))
        ;
      fd = open("test", O_WRONLY);
      write(fd, pos, end - pos);
      close(fd);
    }
  }

  send(client_socket, header, sizeof(header), 0);
  int fd = open("index.html", O_RDONLY);

  bs = splice(fd, 0, pipefds[1], 0, BS, 0);
  splice(pipefds[0], 0, client_socket, 0, bs, 0);

  close(client_socket);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc < 2)
    return 127;
  socket_init(atoi(argv[1]));

  while (1) {
    int client_socket = accept(server_socket, &(struct sockaddr){}, &SOCKLEN);
    pthread_t thread_id;
    pthread_create(&thread_id, 0, client_handle, &client_socket);
    pthread_detach(thread_id);
  }
}
