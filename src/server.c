/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

	FILE *fp;		// Filepointer, Pruefen ob Dateien existieren
	char *aufteilung;

	int sockfd = 0;
	int newsockfd = 0;
	int portno = 0;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n = 0;
	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	  error("ERROR on binding");

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	for(;;)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		//BEGIN get active socket port
		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		if (getsockname(newsockfd, (struct sockaddr *)&sin, &len) == -1)
		{
			error("getsockname");
		}
		else
		{
			printf("port number %d\n", ntohs(sin.sin_port));
		}
		//END get active socket port

		if (newsockfd < 0)
		{
			error("ERROR on accept");
		}

		bzero(buffer,256);

		//DATEN VOM CLIENT EMPFANGEN
		n = read(newsockfd,buffer,255);

		if (n < 0) error("ERROR reading from socket");

		printf("Here is the message: %s\n\n",buffer);

		n = write(newsockfd, "I got your message: %s\n",18);

		if (n < 0) error("ERROR writing to socket");

//		n = read(newsockfd,buffer,255);

//		if (n < 0) error("ERROR reading from socket");

//		printf("\nBuffer: %s\n\n", buffer);

		// Buffer wieder aufteilen, pruefen und ausgeben
		aufteilung = strtok(buffer, "@");
		do
		{
			printf("Moeglicher Dateiname gefunden: %s\n", aufteilung);

			fp = fopen(aufteilung, "r");
			if(fp == NULL)
			{
					printf("Die Datei %s existiert NICHT.\n\n", aufteilung);
			} else
			{
					printf("Die Datei %s existiert.\n\n", aufteilung);
					fclose(fp);
			}

			aufteilung = strtok(NULL, "@");

		} while(aufteilung != NULL);

		close(newsockfd);
	}
	close(sockfd);
	return 0;
}
