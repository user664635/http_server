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

static inline void get_handle(int client_socket, char *buf) {
  static const char header[] =
      "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  send(client_socket, header, sizeof(header), 0);

  int fd = open("index.html", O_RDONLY);
  int pipefds[2];
  pipe(pipefds);
  int bs = splice(fd, 0, pipefds[1], 0, BS, 0);
  splice(pipefds[0], 0, client_socket, 0, bs, 0);
}

#define write_str(fd, str) write(fd, str, sizeof(str))
static inline void post_handle(int client_socket, char *buf, int bs) {
  static const char header[] =
      "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  send(client_socket, header, sizeof(header), 0);

  char *pos = strstr(buf, "boundary=");
  char *end = strstr(pos, "\r\n");
  pos += 9;
  *end++ = 0;
  char *boundary = pos;

  if (!strstr(end, boundary))
    bs += read(client_socket, end = buf + bs, BS);
  if (!(pos = strstr(end, "name=\"user\"")))
    ;
  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  *end++ = '.';
  *end++ = 'c';
  *end++ = 0;
  char *id = pos;
  if (!(pos = strstr(end, "name=\"file\""))) {
    write_str(client_socket, "<p>file upload failed</p>");
    return;
  }
  write_str(client_socket, "<p>file upload success</p>");

  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  int fd;
  if (((fd = creat(id, S_IRUSR | S_IWUSR)) == -1)) {
    write_str(client_socket, "<p>file create failed</p>");
    return;
  }
  write_str(client_socket, "<p>file create success</p>");
  if (write(fd, pos, end - pos) == -1) {
    write_str(client_socket, "<p>file write failed</p>");
    return;
  }
  write_str(client_socket, "<p>file write success</p>");
  close(fd);

  char cmd[64] = "2>&1 clang -c ";
  fd = fileno(popen(strcat(cmd, id), "r"));
  while ((bs = read(fd, buf, BS)) > 0) {
    write(client_socket,buf,bs);
  }
  write_str(client_socket, "<p>complie success</p>");
}

static void *client_handle(void *arg) {
  int bs, client_socket = *(int *)arg;
  char buf[BS];

  bs = read(client_socket, buf, BS);
  buf[bs] = 0;
  puts(buf);

  switch (buf[0]) {
  case 'G':
    get_handle(client_socket, buf);
    break;
  case 'P':
    post_handle(client_socket, buf, bs);
    break;
  }

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
