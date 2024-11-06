#define _GNU_SOURCE

#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BS 65536

#define write_fd(fd, str) write(fd, str, sizeof(str))
#define write_client(str) write_fd(client_socket, str)
static constexpr char head_html[] = {
#embed "head.html"
};

static int server_socket;
static void sigint_handle(int sig) {
  close(server_socket);
  exit(0);
}

static inline void socket_init(int port) {
  if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    exit(1);
  signal(SIGINT, sigint_handle);

  struct sockaddr_in6 server_addr = {};
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_addr = in6addr_any;
  server_addr.sin6_port = htons(port);
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0)
    exit(2);

  if (listen(server_socket, 3) < 0)
    exit(3);
  puts("server init");
}

static inline int post_handle(int client_socket, char *buf, char *file_name,
                              int bs) {
  char *pos = strstr(buf + 20, "boundary=");
  if (!pos)
    return 1;
  char *end = strstr(pos, "\r\n");
  *end++ = 0;
  char *boundary = pos + 9;
  puts(boundary);

  if (!strstr(end, boundary))
    buf[bs += read(client_socket, end = buf + bs, BS - bs)] = 0;

  write_client("user identifying\n");
  if (!(pos = strstr(end, "name=\"user\"")))
    return 2;
  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  if (pos == end)
    return 3;
  *end++ = '/';
  *end++ = 0;

  char path[64] = "data/";
  strcat(path, pos);
  strcat(path, file_name);
  puts(path);

  write_client("file uploading\n");
  if (!(pos = strstr(end, "name=\"file\"")))
    return 4;

  pos = strstr(pos, "\r\n\r\n") + 4;
  end = strstr(pos, boundary) - 4;
  int fd;
  write_client("file creating\n");
  if (((fd = creat(path, S_IRUSR | S_IWUSR)) == -1))
    return 5;

  write_client("file writing\n");
  if (write(fd, pos, end - pos) == -1)
    return 6;
  close(fd);

  char cmd[64] = "2>&1 ./test.sh ";
  fd = fileno(popen(strcat(cmd, path), "r"));
  while ((bs = read(fd, buf, BS)) > 0)
    write(client_socket, buf, bs);

  return 0;
}

static void *client_handle(void *arg) {
  int bs, client_socket = *(int *)arg;
  char buf[BS];

  puts("receiving");
  bs = read(client_socket, buf, BS);
  if (bs <= 0)
    goto exit;
  buf[bs] = 0;
  puts(buf);

  char *pos = strstr(buf, "/");
  *strstr(pos, " HTTP/") = 0;
  char path[64] = "ques";
  strcat(path, pos);
  strcat(path, ".html");
  puts(path);
  int fd = open(path, O_RDONLY);
  if (fd == -1)
    goto exit;

  write_client("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
  write_client(head_html);
  int pipefds[2];
  pipe(pipefds);
  int len = splice(fd, 0, pipefds[1], 0, BS, 0);
  splice(pipefds[0], 0, client_socket, 0, len, 0);

  if (buf[0] == 'P')
    printf("%x\n", post_handle(client_socket, buf, pos, bs));
  write_client("</span></body></html>");

exit:
  close(client_socket);
  puts("connection closed");
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc < 2)
    return 127;
  socket_init(atoi(argv[1]));

  while (1) {
    struct sockaddr_in6 client_addr;
    socklen_t socklen = sizeof(client_addr);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &socklen);
    uint16_t *ip = (uint16_t *)&client_addr.sin6_addr;
    printf("connected from [%x:%x:%x:%x:%x:%x:%x:%x]:%u\n", ip[0], ip[1], ip[2],
           ip[3], ip[4], ip[5], ip[6], ip[7], client_addr.sin6_port);
    pthread_t thread_id;
    pthread_create(&thread_id, 0, client_handle, &client_socket);
    pthread_detach(thread_id);
  }
}
