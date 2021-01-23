#include "unp.h"
#define MAXLINE 256
#define MAXSIZE 256
#define ADDRSIZE 20

int send_over_socket(int output_fd, char *file_name);

int main(int argc, char** argv)
{
	int sockfd, opt, flag;
	struct sockaddr_in servaddr;


	int option_index = 0; 
	char *getoptOptions = "h";
	struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
	};


	char *buffer, *token0, *token1, *token2, *token3, *token4;
	size_t bufsize = MAXLINE; /* variable to be passed to getline() */
	int convert_bit, counter, fp, file_size;
	char addr[ADDRSIZE] = "";
	char text[MAXLINE];
	char user[50];
	char pass[50];
	buffer = (char *)malloc(bufsize * sizeof(char)); /* getline() requires dynamically allocated data for possible resizing */

	if(buffer == NULL)
	{
		fprintf(stdout, "Unable to allocate buffer space\n");
		exit(0);
	}


	 /* parse input from the command line */
	while((opt = getopt_long(argc, argv, getoptOptions, long_options, &option_index)) != -1)
	{
		switch(opt)
		{
			case 'h':
				printf("user should use run.sh script to interface with program.\n");
				return 0;
			case '?':
				printf("\nUsage: \n[-h HELP] [--help]\n");
				return -1;
		}
	}


	/* parse input from stdin */
	bzero(buffer, bufsize); /* clear buffer */
	getline(&buffer, &bufsize, stdin); /* reads stream from stdin to buffer */
	bzero(text, MAXLINE); /* clear text array  */
	strncpy(text, buffer, strlen(buffer));  /* copy the value from buffer into text */

	token1 = strtok(text, " "); /* the space is the delimiter and token1 is the command to be executed */
	if(strcmp(token1, "tconnect") == 0) /* if the command is to connect */
	{
		token2 = strtok(NULL, " ");
		fprintf(stdout, "connect to IP: %s\n", token2);
		strncpy(addr, token2, strlen(token2));
		fprintf(stdout, "addr: %s\n", addr);
		
		
		token3 = strtok(NULL, " ");
		token4 = strtok(NULL, " ");
		if(token3 == NULL || token4 == NULL)
		{
			fprintf(stdout, "not a valid username/password combination\n");
			exit(0);
		}
		strncpy(user, token3, strlen(token3));
		strncpy(pass, token4, strlen(token4));
		fprintf(stdout, "Username: %s; Password: %s\n", user, pass);
	}
	else
		fprintf(stderr, "Please start the code by using tconnect\n");

	

	/* create socket */
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fprintf(stdout, "failed socket creation\n");
	else
		fprintf(stdout, "successful socket creation\n");


	/* specify an address for the socket */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	convert_bit = inet_pton(AF_INET, addr, &servaddr.sin_addr);
	if(convert_bit <= 0)
		{fprintf(stdout, "failed to convert addr: %s... errcode: %d\n", addr, convert_bit);}


	/* connect to the server */
	if(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)) < 0)
		fprintf(stdout, "There was an error making a conection to the remote socket... %s\n\n", strerror(errno));

	/* give server username and password */
	bzero(buffer, MAXLINE);
	sprintf(buffer, "auth %s %s", user, pass);
	fprintf(stdout, "authenticating credentials... %s %s\n", user, pass);
	write(sockfd, buffer, MAXLINE);
	bzero(buffer, MAXLINE);
	read(sockfd, buffer, MAXLINE);
	fprintf(stdout, "server response: %s\n", buffer);
	if(strcmp(buffer, "proceed") != 0)
	{
	close(sockfd);
	fprintf(stdout, "credentials denied :(. client terminating\n");
	exit(0);
	}


	flag = 1;
	while(flag) /* main while loop */
	{
		bzero(buffer, bufsize); 
		getline(&buffer, &bufsize, stdin);
		bzero(text, bufsize);
		strncpy(text, buffer, strlen(buffer));

		token0 = strtok(text, " ");
		fprintf(stdout, "write to server: %s.\n", token0);
		write(sockfd, token0, bufsize); /* write the command to the socket */

		bzero(buffer, bufsize); /* clear the buffer */
		read(sockfd, buffer, bufsize); /* wait for the socket to respond */
		fprintf(stdout, "server response: %s\n", buffer);

		token1 = strtok(text, " ");
		if(strlen(token1) > 0)
		{
			switch (token1[1])
			{
			case 'g':
				if(strcmp(token1, "tget") == 0)
				{
					token2 = strtok(NULL, " ");

					fprintf(stdout, "creating file with name: %s\n", token2);
					fp = open(token2, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0644);
					if(fp == -1) 
					{
						if(errno == EEXIST)
						{
							fprintf(stdout, "client already has this file! aborting download request.\n");
						}
						else
						{
							fprintf(stdout, "file could not be created...\n");
						}
						write(sockfd, "cancel", 64); /* cancel the file transfer */
						break;
					}
					else
					{
						write(sockfd, "proceed", 64); /* tell the server to continute */
					}
					

					bzero(buffer, MAXLINE);
					read(sockfd, buffer,bufsize);
					if(strcmp(buffer, "ENOENT") == 0)
					{
						fprintf(stdout, "the requested file cannot be found\n");
						close(fp);
						remove(token2);
						break;
					}
					if(strcmp(buffer, "err") == 0)
					{
						fprintf(stdout, "there was an error retriving the file\n");
						close(fp);
						remove(token2);
						break;
					}
					/* if the server message was not an error, we assume its the file size */
	
					file_size = atoi(buffer);
					fprintf(stdout, "file_size = %d\n", file_size);
					
					if(file_size == 0)
					{
						write(sockfd, "ACK", 64); /*this might cause an error*/
						close(fp);
						fprintf(stdout, "file recieved\n");
						break;
					}
					
					counter = 0;
					while((counter * MAXLINE) < file_size)
					{
						bzero(buffer, MAXLINE);
						write(sockfd, "ACK", 64); /* ready for data chunks */
						read(sockfd, buffer, MAXLINE);
						if(((counter+1)*MAXLINE) > file_size) /* lets not load the file with extra data or null chars*/
						{
							write(fp, buffer, file_size%(counter*MAXLINE));
						}
						else
						{
							write(fp, buffer, MAXLINE);
						}
						counter++;
					}
					fprintf(stdout, "count is: %d and should be %d\n", counter, (file_size/MAXLINE+1));
					close(fp);
					fprintf(stdout, "file transfered\n");
					write(sockfd, "ACK", 64);





				}
				else
					fprintf(stdout, "zobe\n");
				break;

			case 'p':
				if(strcmp(token1, "tput") == 0)
				{
					token2 = strtok(NULL, " ");
					if(strcmp(buffer, "exist") == 0)
					{
						fprintf(stdout, "file exists at the server! aborting command.\n");
						break;
					}
					if(strcmp(buffer, "err") == 0)
					{
						fprintf(stdout, "there was an error with the server creating the file\n");
						break;
					}

					if(send_over_socket(sockfd, token2) == -1)
						fprintf(stdout, "there was an issue sending the file. errno: %s\n", strerror(errno));
					else
						fprintf(stdout, "The file should have sent over the socket\n");

				}
				else
					fprintf(stdout, "wobe\n");
				break;
			case 'x':
				if(strcmp(token1, "exit") == 0)
				{
					flag = 0; /* break the loop and close the socket */
				}
				break;
			default:
				fprintf(stderr, "%s - Not a valid input, program terminating\n", token1);
				flag = 0;
			}
		}


	}

	close(sockfd);
	fprintf(stdout, "\nclient terminating\n");
	return 0;
}

int send_over_socket(int output_fd, char *file_name)
{
	struct stat obj;
	int fp, file_size, value, bytes_read;
	char send_buff[MAXSIZE] = "";
	
	fp = open(file_name, O_RDONLY);
	if(fp == -1)
	{
		fprintf(stdout, "s_o_s err: open() - %s\n", strerror(errno));
		return -1;
	}

	stat(file_name, &obj);
	file_size = obj.st_size;
	sprintf(send_buff, "%d", file_size);
	write(output_fd, send_buff, sizeof(obj));
	bzero(send_buff, MAXSIZE);
	read(output_fd, send_buff, MAXSIZE); /*wait for ack */	



	bzero(send_buff, MAXSIZE);
	while((bytes_read = read(fp, send_buff, MAXSIZE)) > 0)
	{
		value = write(output_fd, send_buff, bytes_read);
		bzero(send_buff, MAXSIZE);
		read(output_fd, send_buff, MAXSIZE);
		bzero(send_buff, MAXSIZE);
	}

	close(fp);

	return value;
}
