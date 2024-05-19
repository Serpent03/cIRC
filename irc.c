#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include "irc.h"

i32 read_socket(irc_socket *irc) {
  /* there's probably a better solution for this;
   * but this basically ensures all previous leftover data
   * has been reset. */
  memset(irc->buf, 0, SOCKBUFFLEN); 
  return recv(irc->sockfd, irc->buf, SOCKBUFFLEN, 0);
}

void write_socket(irc_socket *irc, const char* msg) {
  send(irc->sockfd, msg, strlen(msg), 0);
  printf("\n>>>%s", msg);
}

void reply_ping(irc_socket *irc) {
  /* The PING and PONG formats(example below) are the same.
   * PING :eecAWH{}]
   * PONG :eecAWH{}] 
   * Thus, a simple strncpy works wonders. */
  strncpy(irc->buf, "PONG", 4);
  write_socket(irc, irc->buf);
}

irc_socket * init_irc_socket(char *server_addr, u16 port) {
  irc_socket *irc = (irc_socket*)calloc(1, sizeof(irc_socket));
  irc->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (irc->sockfd < 0) {
    /* in the case we are not able to open a socket file descriptor. */
    perror("Error on socket init");
    close(irc->sockfd);
    free(irc);
    return NULL;
  }

  memset(&irc->saddr, 0, sizeof(irc->saddr));
  irc->saddr.sin_family = AF_INET;
  irc->saddr.sin_port = htons(port);
  irc->saddr.sin_addr.s_addr = inet_addr(server_addr);
  
  if (connect(irc->sockfd, (struct sockaddr*)&irc->saddr, sizeof(irc->saddr)) < 0) {
    /* In the case that we are not able to connect. */
    perror("Error on socket connect");
    close(irc->sockfd);
    free(irc);
    return NULL;
  }
  return irc;
}

void deinit_irc_socket(irc_socket *irc) {
  close(irc->sockfd);
  free(irc);
  irc = NULL;
}

int main(int argc, char **argv) {
  // if (argc < 3) {
  //   /* We're expecting one server address and one port input */
  //   printf("Incorrect arguments!");
  //   exit(1);
  // }
  
  irc_socket *irc = init_irc_socket("207.148.28.126", 6667);
  if (!irc) {
    /* The allocation has been unsuccesful. Return an error. */
    exit(1);
  }

  /* connecting to the IRC up until this point.
   * need to determine the actual IP address from the IRC 
   * server/host name */
  /* https://www.rfc-editor.org/rfc/rfc2812.txt#:~:text=to%20Kilroy.%0A%0A3.1.3-,User%20message,-Command%3A%20USER%0A%20%20%20Parameters */

  read_socket(irc);
  write_socket(irc, "NICK foo\n");
  write_socket(irc, "USER foo 8 * :foo\n");
  while (true) {
    read_socket(irc);
    printf("%s", irc->buf);
    if (strncmp(irc->buf, "PING", 4) == 0) {
      /* at this point, we're receiving a PING, so we reply to it */
      reply_ping(irc);
      break;
    }
  }
  /* CONNECTION SHOULD BE ESTABLISHED AT THIS STAGE */
  read_socket(irc);
  printf("CONN_REPLY\n%s\n", irc->buf);

  /* At this point, it should be in a loop, just reading
   * all the relevant incoming messages. I should be able to 
   * enter commands and other things from my keyboard, which should 
   * then be sent straight to the write_socket() function. */

  write_socket(irc, "QUIT\n");
  deinit_irc_socket(irc);
  return 0;
}
