#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define FILENAME_START 4

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void printError(const char *msg)
{
    fprintf(stderr, "%s", msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd = 0;
	int portno = 0;
	int n = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;

    int buffersize = 0;
    char status[50];
    char *buffer = NULL;
    char *fileBytes = calloc(1, 10 * sizeof(char)+1);

	//Check for arguments
    if (argc < 4)
    {
    	printError("usage %s hostname port numberOfBytes filenames\n");
    }

    //Check for control character in filename
    for(int i = FILENAME_START; i < argc; i++) {
        if(strstr(argv[i], "<"))
        {
            printError("< is not a valid filename character!\n");
            
        }
    }

    //Check if N bytes is valid
    if(strlen(argv[3]) > 2)
	{
		printError("Invalid number of bytes!\n");
	}

    for(int i = 0; i < strlen(argv[3]); i++)
	{
        if(!(argv[2][i] >= '0' && argv[3][i] <= '9'))
        {
        	printError("Invalid number of bytes!!\n");
        }
	}

    if(atoi(argv[3]) > 10) {
        printError("Invalid number of bytes!!\n");
    }

	//Check and convert portnumber to int
	if(strlen(argv[2]) > 5)
	{
		printError("Invalid portnumber!\n");
	}

	for(int i = 0; i < strlen(argv[2]); i++)
	{
        if(!(argv[2][i] >= '0' && argv[2][i] <= '9'))
        {
        	printError("Invalid portnumber!\n");
        }
	}

    portno = atoi(argv[2]);

    if(portno > 65535)
    {
    	printError("Invalid portnumber!\n");
    }
    
    printf("Port: %d\n", portno);

    //Initialize structure with \0
    memset(&serv_addr, '\0', sizeof(serv_addr));
    memset(status, '\0', sizeof(status));

    //Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sockfd: %d\n", sockfd);
    if (sockfd < 0)
    {
    	error("ERROR opening socket");
    }

    //Resolve hostname
    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
    	printError("ERROR, no such host\n");
    }

    //Fill needed structure with information
    printf("Server: %s\n", server->h_name);
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);

    //Make IP printable
    char ipaddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, ipaddr, INET_ADDRSTRLEN);
    printf("IP: %s\n", ipaddr);

    //Connect
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
    	error("ERROR connecting");
    }
    
    //Send number of bytes
    n = write(sockfd, argv[3], strlen(argv[3]));
    usleep(100);

    n = read(sockfd, status, sizeof(status));
    if(strcmp("OK", status)) {
        printf("%s\n", status);
        printError("Something went wrong!\n");
    }

    //Calculate buffersize
    for(int i = FILENAME_START; i < argc; i++)
    {
    	buffersize = buffersize + strlen(argv[i])+1;
    }
    printf("Buffersize: %d\n", buffersize);

    //Allocate buffer
    buffer = calloc(1, buffersize * sizeof(char)+1);
	if(buffer == NULL)
	{
		printf("Something went wrong while allocating memory!\n");
		exit(1);
	}

    //Fill buffer
    for(int i = FILENAME_START; i < argc; i++)
    {
    	strcat(buffer, argv[i]);
    	strcat(buffer, "<");
    }
    buffer[strlen(buffer)-1] = '\0';
    printf("SendBuffer: %s\n", buffer);

    //Send buffer
    n = write(sockfd, buffer,strlen(buffer));
	if (n < 0)
	{
		error("ERROR writing to socket");
	}

	memset(buffer, '\0', buffersize);
	free(buffer);

	buffersize = 255 * sizeof(char)+1;
	buffer = calloc(1, buffersize);

	n = read(sockfd,buffer,buffersize);
	printf("ReceiveBuffer: %s\n\n", buffer);

	memset(buffer, '\0',  buffersize);
	n = read(sockfd, buffer, buffersize);
    printf("Buffer: %s\n\n", buffer);

	//Output
	for(int i = 0; i < strlen(buffer); i++)
	{
		if(buffer[i] == '0')
		{
			printf("Die Datei %s existiert NICHT.\n", argv[i+FILENAME_START]);
		} else if(buffer[i] == '1')
		{
			printf("Die Datei %s existiert.\n", argv[i+FILENAME_START]);
            memset(fileBytes, '\0', buffersize);
            n = read(sockfd, fileBytes, buffersize);
            printf("Inhalt %s: %s\n", argv[i+FILENAME_START], fileBytes);
		}
        printf("\n");
	}

	free(buffer);

    return 0;
}