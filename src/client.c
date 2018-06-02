#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void printError(const char *msg) {
    fprintf(stderr, "%s", msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd = 0;
	int portno = 0;
	int n = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;

    int buffersize = 0;
    char *buffer = NULL;

	//Check for arguments
    if (argc < 4) {
       printError("usage %s hostname port filenames\n");
    }

	//Convert portnumber to int
	if(strlen(argv[2]) > 5) {
		printError("Invalid portnumber!\n");
	}

	for(int i = 0; i < strlen(argv[2]); i++) {
        if(!(argv[2][i] >= '0' && argv[2][i] <= '9')){
            printError("Invalid portnumber!\n");
        }
	}

    portno = atoi(argv[2]);

    if(portno > 65535) {
        printError("Invalid portnumber!\n");
    }
    
    printf("Port: %d\n", portno);

    //Initialize structure with \0
    memset(&serv_addr, '\0', sizeof(serv_addr));

    //Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sockfd: %d\n", sockfd);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    //Resolve hostname
    server = gethostbyname(argv[1]);
    if(server == NULL) {
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
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    //Calculate buffersize
    for(int i = 3; i < argc; i++) {
        buffersize = buffersize + strlen(argv[i])+1;
    }
    printf("Buffersize: %d\n", buffersize);

    //Allocate buffer
    buffer = calloc(1, buffersize * sizeof(char)+1);
	if(buffer == NULL) {
		printf("Something went wrong while allocating memory!\n");
		exit(1);
	}

    //Fill buffer
    for(int i = 3; i < argc; i++) {
        strcat(buffer, argv[i]);
        strcat(buffer, "@");
    }
    printf("Buffer: %s\n", buffer);

    //Send buffer
    n = write(sockfd, buffer,strlen(buffer));
	if (n < 0) {
		error("ERROR writing to socket");
	}

	free(buffer);

    return 0;
}
