/*A simple server in the internet domain using TCP
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
    int anzahl = 0;
    char invalidNumBytes[] = "NOK! Invalid number of bytes!";

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
        char invalid = 0;
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
			printf("Incoming connection at port: %d\n", ntohs(sin.sin_port));
		}
		//END get active socket port

		if (newsockfd < 0)
		{
			error("ERROR on accept");
		}

        memset(buffer, '\0', sizeof(buffer));

        //Receive number of bytes to be read and sanitize them
        n = read(newsockfd, buffer, 2);

		printf("N is equal to: %d\n", n);

        for(int i = 0; i < strlen(buffer); i++)
	    {
            if((buffer[i] >= '0' && buffer[i] <= '9'))
            {
				anzahl = atoi(buffer);
				if (anzahl > 10) {
                	printf("Too many bytes to be read!\n");
                	write(newsockfd, invalidNumBytes, strlen(invalidNumBytes));
                	invalid = 1;
					break;
            	}
			}
			else {
				printf("Invalid number of bytes!\n");
                printf("Received Data: %s\n", buffer);
                write(newsockfd, invalidNumBytes, strlen(invalidNumBytes));
                invalid = 1;
				break;
			} 
	    }

        printf("\n");

        if(!invalid) {
			n = write(newsockfd, "OK", 2);
			printf("Number of bytes: %d\n", anzahl);

			memset(buffer, '\0', sizeof(buffer));

			//DATEN VOM CLIENT EMPFANGEN
			n = read(newsockfd,buffer,255);

			if (n < 0) error("ERROR reading from socket");

			printf("Here is the message: %s\n\n",buffer);

			n = write(newsockfd, "I got your message.\n",255);

			if (n < 0) error("ERROR writing to socket");

			// Buffer wieder aufteilen, pruefen und ausgeben
			/** TOFIX **/
			int i = 0; // Laufvariable
			char newbuffer[256];
			char outputString[10][20];
			memset(newbuffer, '\0', sizeof(newbuffer));
			/** TOFIX **/
			aufteilung = strtok(buffer, "<");
			do
			{
				printf("Moeglicher Dateiname gefunden: %s\n", aufteilung);

				fp = fopen(aufteilung, "r");
				if(fp == NULL)
				{
						printf("Die Datei %s existiert NICHT.\n\n", aufteilung);
						newbuffer[i] = '0';
				} else
				{
						printf("Die Datei %s existiert.\n\n", aufteilung);
						newbuffer[i] = '1';
						fgets(outputString[i], anzahl+1, fp);
						fclose(fp);
				}

				aufteilung = strtok(NULL, "<");
				i++;

			} while(aufteilung != NULL);

			n = write(newsockfd, newbuffer, 255);
			usleep(1000);

			for(int j = 0; j < i; j++) {
				if(newbuffer[j] == '1') {
				n = write(newsockfd, outputString[j], strlen(outputString[j]));
				printf("String: %s\n", outputString[j]);
				usleep(5000);
				}
			}
			printf("-----------------------------\n");

			if (n < 0) error("ERROR writing to socket");
		}
		close(newsockfd);
        }

	close(sockfd);
	return 0;
}
