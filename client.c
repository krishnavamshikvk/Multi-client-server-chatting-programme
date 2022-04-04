#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>




void *client_read(void *arg)
{
	int sockfd = *(int *)arg;
	int tmp_n;
	while (1)
	{
		char buffer[1000];
		memset(buffer, 0, sizeof(buffer));
		tmp_n = read(sockfd, buffer, 1000);
		if (strcmp(buffer, "goodbye\n") == 0)
		{
			write(sockfd, "goodbye\n", sizeof("goodbye\n"));
		}
		if (tmp_n == 0)
		{
			break;
		}
		printf("%s" ,buffer);
	}
	printf("\t\t\t Disconnected please restart if this was not intentional.\n");
	pthread_exit(NULL);
}

int main(int argc,char *argv[])
{
	int sockfd, n,portno;
	char buffer[1000];
	struct sockaddr_in servaddr;
        struct hostent *server;
	if(argc < 3){
        fprintf(stderr,"usage %s hostname port\n",argv[0]);
        exit(1);
    }
    
    portno =atoi(argv[2]);
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	  if(sockfd < 0)
        perror("ERROR opening socket.");
    	
     server = gethostbyname(argv[1]);
     if(server == NULL){
        fprintf(stderr,"Error no host exits.");
    }
      bzero((char *) &servaddr,sizeof(servaddr));

	
	servaddr.sin_family = AF_INET;
	  bcopy((char *) server->h_addr, (char *)&servaddr.sin_addr.s_addr,server->h_length);
	servaddr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("Connect");
		exit(1);
	}
	printf("Connected\n");
	pthread_t tid;
	pthread_create(&tid, NULL, client_read, &sockfd);
	while (1)
	{
		
		sleep(1);
		fgets(buffer, 1000, stdin);
		write(sockfd, buffer, sizeof(buffer));
		if (strcmp(buffer, "close\n") == 0)
			break;
	}
	return 0;
}
