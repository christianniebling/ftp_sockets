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

#define LISTENQ 5
#define PORT 9002
#define SA struct sockaddr

char *removeNewline(char *str)
{	/* removes newline from str. usage: str = removeNewline(str); */
	char *temp;
	temp = strtok(str, "\n");
	return temp;
}
