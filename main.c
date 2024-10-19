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

#define write_fd(fd, str) write(fd, str, sizeof(str))
#define write_client(str) write_fd(client_socket, str)

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

  int fd = open("index.html", O_RDONLY);
  int pipefds[2];
  pipe(pipefds);
  int bs = splice(fd, 0, pipefds[1], 0, BS, 0);
  splice(pipefds[0], 0, client_socket, 0, bs, 0);
}

static inline void post_handle(int client_socket, char *buf, int bs) {
  write_client("<span style=\"white-space:pre-line\">");

  char *pos = strstr(buf, "boundary=");
  if (!pos)
    goto exit;
  char *end = strstr(pos, "\r\n");
  *end++ = 0;
  char *boundary = pos + 9;

  if (!strstr(end, boundary))
    bs += read(client_socket, end = buf + bs, BS);

  write_client("user identifying\n");
  if (!(pos = strstr(end, "name=\"user\"")))
    goto exit;
  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  if (pos == end)
    goto exit;
  *end++ = '/';
  *end++ = 0;
  char path[64] = "data/";
  char file_name[] = "test.c";
  strcat(path, pos);
  strcat(path, file_name);

  write_client("file uploading\n");
  if (!(pos = strstr(end, "name=\"file\"")))
    goto exit;

  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  int fd;
  write_client("file creating\n");
  if (((fd = creat(path, S_IRUSR | S_IWUSR)) == -1))
    goto exit;

  write_client("file writing\n");
  if (write(fd, pos, end - pos) == -1)
    goto exit;
  close(fd);

  char cmd[64] = "2>&1 ./test.sh ";
  fd = fileno(popen(strcat(cmd, path), "r"));
  while ((bs = read(fd, buf, BS)) > 0)
    write(client_socket, buf, bs);

exit:
  write_client("\nexiting</span>");
}

static void *client_handle(void *arg) {
  int bs, client_socket = *(int *)arg;
  char buf[BS];

  bs = read(client_socket, buf, BS);
  buf[bs] = 0;
  puts(buf);

  write_client("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
               "<!DOCTYPE html><html><head>"
               "<meta charset =\"utf-8\">"
               "<title>test</title>"
               "</head><body>");

  write_client("<form enctype=\"multipart/form-data\" method=\"post\">"
               "<input type=\"text\" name=\"user\" />"
               "<input type=\"file\" name=\"file\" />"
               "<input type=\"submit\" />"
               "</form></body></html>");
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
