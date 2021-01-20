#include "unp.h"
#include <sys/stat.h>
#include <fcntl.h>
#define MAXLINE 256
#define MAXSIZE 256

int send_over_socket(int output_fd, char *file_name);

int main(int argc, char** argv)
{
	FILE *fptr;
	int listenfd, connfd, file_size, fp, counter, verified;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char buff[MAXLINE];
	char text[MAXLINE];
	char user [50] = "";
	char pass [50] = "";
	char cuser [50] = "";
	char cpass [50] = "";
	char *token1, *token2, *b;
	size_t bufsize = MAXLINE;
	b = buff;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* initalize the uint containing the server address and port */
	bzero(&servaddr, sizeof(servaddr));	/* set bytes to zero */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);		
	servaddr.sin_port = htons(PORT);
	
	/* specifies in the IP header the server address and port */
	bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
	

	listen(listenfd, LISTENQ);
	
	for ( ; ; ) 
	{
		len = sizeof(cliaddr);

		connfd = accept(listenfd, (SA*) &cliaddr, &len);
		printf("connection from %s, port %d\n",
			inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
			ntohs(cliaddr.sin_port));


		/* get the username and password */
		bzero(buff, MAXLINE);
		bzero(text, MAXLINE);
		read(connfd, buff, MAXLINE);
		strncpy(text, buff, strlen(buff));
		strtok(text, " ");
		token1 = strtok(NULL, " ");
		token2 = strtok(NULL, " ");
		strncpy(user, token1, strlen(token1));
		strncpy(pass, token2, strlen(token2));
		fprintf(stdout, "username: %s. \npassword: %s.\n", user, pass);

		fptr = fopen("secret.txt", "r");
		bzero(buff, MAXLINE);
		bzero(text, MAXLINE);
		verified = 0;
		while(getline(&b, &bufsize, fptr) != -1)
		{
			strncpy(text, buff, strlen(buff));
			token1 = strtok(text, ",");
			token2 = strtok(NULL, ";");
			strncpy(cuser, token1, strlen(token1));
			strncpy(cpass, token2, strlen(token2));
			if(strcmp(user, cuser) == 0 && strcmp(pass, cpass) == 0)
				verified = 1;
			bzero(buff, MAXLINE);
			bzero(text, MAXLINE);
			bzero(cuser, 50);
			bzero(cpass, 50);
		}
		
		if(verified)
		{
			fprintf(stdout, "user authentified\n");
			write(connfd, "proceed", 64);
		}
		else
		{
		fprintf(stdout, "user has no right and he should go home\n");
		write(connfd, "no dice", 64);
		close(connfd);
		continue;
		}
		


		
		do
		{

			bzero(buff, MAXLINE); 
			read(connfd, buff, MAXLINE);
			fprintf(stdout, "recieved message:%s.\n", buff);
			/* write(connfd, "ACK", 64); */
			

			bzero(text, MAXLINE);
			strncpy(text, buff, strlen(buff));
			token1 = strtok(text, " ");
			if(strlen(token1) > 1)
			{
				switch (token1[1])
				{
				case 'g':
					if(strcmp(token1, "tget") == 0)
					{
						token2 = strtok(NULL, " ");
						fprintf(stdout, "client requested a file\n");
						write(connfd, "ACK", 64);
						bzero(buff, MAXLINE);
						read(connfd, buff, MAXLINE);
						
						if(strcmp(buff, "cancel") == 0)
						{
							fprintf(stdout, "server cancelled get request\n");
							break;
						}
						
						if(send_over_socket(connfd, token2) == -1)
							fprintf(stdout, "there was an issue sending the file. errno: %s\n", strerror(errno));
						else
							fprintf(stdout, "The file should have sent over the socket\n");
						
						/*write(connfd, "ACK", 64);*/
					}
					else
						fprintf(stdout, "zobe\n");
					break;

				case 'p':
					if(strcmp(token1, "tput") == 0) /*server is recieving file */
					{
						token2 = strtok(NULL, " "); /* name of file */
						fprintf(stdout, "creating file with name: %s\n", token2);
						fp = open(token2, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0644);
						if(fp == -1)
						{
							if(errno == EEXIST)
							{
								fprintf(stdout, "file already exists! aborting upload request\n");
								/* tell the client he's lame */
								write(connfd, "exist", 64);
							}
							else
							{
								fprintf(stdout, "file could not be created...\n");
								write(connfd, "err", 64);
							}
							break;
						}

						
						write(connfd, "ACK", 64); /* if no error, ready for file size */
						bzero(buff, MAXLINE);
						read(connfd, buff, MAXLINE);
		
						file_size = atoi(buff);
						fprintf(stdout, "file_size = %d\n", file_size);
						if(file_size == 0)
						{
							close(fp);
							fprintf(stdout, "file transfered\n");
							write(connfd, "ACK", 64);
							break;
						}
				
						counter = 0;
						while((counter * MAXLINE) < file_size)
						{
							bzero(buff, MAXLINE);
							write(connfd, "ACK", 64); /* ready for data chunks */
							read(connfd, buff, MAXLINE);
							if(((counter+1)*MAXLINE) > file_size) /* lets not load the file with extra data or null chars*/
							{
      								write(fp, buff, file_size%(counter*MAXLINE));
								fprintf(stdout, "wrote remaining %d bytes\n", (counter*MAXLINE));
							}
							else
							{
								write(fp, buff, MAXLINE);
							}
							counter++;
						}
						close(fp);
						fprintf(stdout, "file transfered\n");
						write(connfd, "ACK", 64);

					}
					else
						fprintf(stdout, "wobe\n");
					break;
				case 'x':
					if(strcmp(token1, "exit") == 0)
					{
						fprintf(stdout, "exit recieved\n");
						write(connfd, "exit_ACK", 64);
					}
					break;
				default:
					fprintf(stdout, "%s - recieved an unrecgonized command. do nothing\n", token1);
					write(connfd, "ACK", 64);
				}
				
			}



			
		}while(strcmp(token1, "exit") != 0);
		
		
		close(connfd);
		fprintf(stdout, "client connection has been terminated\n");
	}
}


int send_over_socket(int output_fd, char *file_name)
{
	struct stat obj;
	int fp, file_size, value, bytes_read, count;
	char send_buff[MAXSIZE] = "";
	value = 0;

	fp = open(file_name, O_RDONLY);
	if(fp == -1)
	{
		if(errno == ENOENT)
		{
			fprintf(stdout, "the requested file was not found\n");
			write(output_fd, "ENOENT", 64);
		}
		else
		{
			fprintf(stdout, "s_o_s err: open() - %s\n", strerror(errno));
			write(output_fd, "err", 64);
		}
		return -1;
	}
	stat(file_name, &obj);
	file_size = obj.st_size;
	sprintf(send_buff, "%d", file_size);
	write(output_fd, send_buff, sizeof(obj));
	bzero(send_buff, MAXSIZE);
	read(output_fd, send_buff, MAXSIZE); /*wait for ack */
	
	if(file_size == 0)
	{
		close(fp);
		return 0;
	}
	
	fprintf(stdout, "starting to send data\n");
	bzero(send_buff, MAXSIZE);
	count = 0;
	while((bytes_read = read(fp, send_buff, MAXSIZE)) > 0)
	{
		value = write(output_fd, send_buff, bytes_read);
		bzero(send_buff, MAXSIZE);
		read(output_fd, send_buff, MAXSIZE);
		bzero(send_buff, MAXSIZE);
		count++;
	}
	fprintf(stdout, "count: %d\n", count);
/*
	counter = 0;
	while((counter * MAXSIZE) <= file_size)
	{
		bzero(send_buff, MAXSIZE);
		value = read(fp, send_buff, MAXSIZE); 
		if(value == -1) 
			fprintf(stdout, "failed to read from file. errno: %s\n", strerror(errno)); 
		value = write(output_fd, send_buff, MAXSIZE); 
		bzero(send_buff, MAXSIZE);
		read(output_fd, send_buff, MAXSIZE); 
		counter++;
	}
*/
	close(fp);
	return value;
}
