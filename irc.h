#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

/* === DEFINITIONS === */

#define SOCKBUFFLEN 4096

typedef unsigned char u8;
typedef char i8;

typedef unsigned short int u16;
typedef short int i16;

typedef unsigned int u32;
typedef int i32;


/* === STRUCT & METHODS === */

typedef struct irc_socket {
  i32 sockfd;
  struct sockaddr_in saddr;
  char buf[4096];
} irc_socket;

/**
 * @brief Initialize the IRC socket.
 * @param server_addr The IP address of the IRC server.
 * @param port TCP port of the IRC server.
 * @return A pointer to an irc_socket instance if everything is successful, otherwise NULL.
 */
irc_socket * init_irc_socket(char *server_addr, u16 port);

/**
 * @brief De-initialize the IRC socket and close the socketfd.
 * @param irc The IRC_SOCKET struct in question.
 */
void deinit_irc_socket(irc_socket *irc);

/**
 * @brief Read incoming packets on the socket.
 * @param sockfd Socket file descriptor.
 * @param buf Copies bytes into the buffer.
 * @return A positive value if something has been received.
 */
i32 read_socket(irc_socket *irc);

/**
 * @brief Write packets onto the open socket.
 * @param sockfd Socket file descriptor.
 * @param msg The bytes to write.
 */
void write_socket(irc_socket *irc, const char *msg);

/**
 * @brief Reply to a PING message from the IRC server.
 * @param sockfd Socket file descriptor.
 * @param buf Buffer containing reply sequence.
 */
void reply_ping(irc_socket *irc);

bool join_channel(irc_socket *irc, const char *channel);
