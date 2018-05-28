#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd = 0;
	int portno = 0;
	int n = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //Neue Variablen fue die String-Bearbeitung
    int groesse_alt[argc-3];	//Dateinamen, Zeichenanzahl
	int groesse_neu[argc-3];	//Dateinamen, Zeichenanzahl mit Trennzeichen
	char *dateinamen[argc-3];

	int buffergroesse = 0;
	char *buffer;

	int i = 3;		// Zaehlvariable
	int j = 0;		// Extra Zaehlvariable

    if (argc < 3)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //KOMMUNIKATION - ANFANG
    printf("port:%d\n\n", serv_addr.sin_port);

	//Anzahl Zeichen der Dateinamen bestimmen und um 1 erweitern
	while (i < argc)
	{
		groesse_alt[j] = strlen(argv[i]);
		printf("%d.Name: %s, ALT: %d.Zeichen\n", j, argv[i], groesse_alt[j]);

		groesse_neu[j] = groesse_alt[j]+1;
		printf("%d.Name: %s, NEU: %d.Zeichen\n", j, argv[i], groesse_neu[j]);

		dateinamen[j] = malloc(groesse_neu[j] * sizeof(char));
		if(dateinamen[j] != NULL)
		{
			printf("Speicher fuer Dateiname ist reserviert\n\n");
		}else
		{
			printf("Kein freier Speicher vorhanden.\n\n");
		}
		i++;
		j++;
	}
	i = 3;
	j = 0;

	// Dateinamen von der Main-Funktion in neue Arrays kopieren
	while(i < argc)
	{
		strcpy(dateinamen[j], argv[i]);

		strcat(dateinamen[j], "@");
		printf("%d.Name: %s\n", j, dateinamen[j]);
		i++;
		j++;

	}
	i = 3;
	j = 0;


	//Buffergroesse ermitteln
	while(i < argc)
	{
		buffergroesse = buffergroesse + groesse_neu[j];
		i++;
		j++;
	}
	i = 3;
	j = 0;

	printf("Buffergroesse: %d\n", buffergroesse);
	buffer = malloc(buffergroesse * sizeof(char)+1);

	if(buffer != NULL)
	{
		printf("\nSpeicher fuer Buffer ist reserviert\n");
	}else
	{
		printf("\nKein freier Speicher vorhanden.\n");
	}

	bzero(buffer,strlen(buffer));

	//Dateinamen in buffer zusammenfuegen
	while(i < argc)
	{
		strcat(buffer, dateinamen[j]);
		i++;
		j++;
	}
	i = 3;
	j = 0;

	printf("\nBuffer: %s\n\n", buffer);

	//DATEINAMEN AN SERVER SENDEN
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	{
		error("ERROR writing to socket");
	}

	bzero(buffer,strlen(buffer));
	n = read(sockfd,buffer,255);
	if (n < 0)
	{
		error("ERROR reading from socket");
	}

	printf("%s\n",buffer);
	bzero(buffer,strlen(buffer));
	fgets(buffer,strlen(buffer),stdin);
	n = write(sockfd,buffer,strlen(buffer));

	if (n < 0)
	{
		error("ERROR writing to socket");
	}


	//ALLOCIERTEN SPEICHER FREIGEBEN
	for(int i = 0; i < argc-3; i++)
	{
		free(dateinamen[i]);
		printf("Speicher der Dateinamen freigegeben!!!\n");
	}

	free(buffer);
	printf("Buffer freigegeben!!!\n");

	close(sockfd);
	return 0;
}
