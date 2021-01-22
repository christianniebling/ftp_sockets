#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>
#include <getopt.h>

#include <arpa/inet.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>

#define LISTENQ 25
#define PORT 9002
#define SA struct sockaddr
