#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

int client_count = 0;

struct client
{
	int index;
	int sockfd;
	char status[20];
	int dest_sockfd;
};
struct client clients[200];

void *thread_chat(void *args)
{
	int index = *(int *)args;  // struct client *cl = (struct client *)args;
	char buffer[1000], hello[1000] = "Hello from server!\nThe syntax of commands is as follows: \n close - to exit\n get - to get the list of all available clients\n chat (client_id) - to connect to particular client\n";
	int n, tmp_n;
	write(clients[index].sockfd, hello, sizeof(hello));
	while (1)
	{
		memset(buffer, 0, 1000);
		n = read(clients[index].sockfd, buffer, sizeof(buffer));
		printf("command from client %d:%s\n", index, buffer);
		printf("current number of clients %d\n", client_count);
		if (strcmp(buffer, "close\n") == 0 || n == 0)
		{
			printf("Client %d disconnected\n", index);
			if (strcmp(clients[index].status, "busy") == 0)
			{
				write(clients[index].dest_sockfd, "goodbye\n", sizeof("goodbye\n"));
			}
			break;
		}
		else if (strcmp(clients[index].status, "free") == 0)
		{
			// client is free and can use get/chat
			if (strcmp(buffer, "get\n") == 0)
			{
				// send list of all the clients
				printf("Sending list of all clients\n");
				int l = 0;
				char output[1000], tmp[1000] = "list of clients:\n";
				memset(output, 0, 1000);
				for (int i = 0; i < client_count; i++)
				{
					printf("current client: %d:%s\n", i, clients[i].status);
					if (i != index)
					l += snprintf(output + l, 1000 - l, "client %d is %s\n", clients[i].index, clients[i].status);
				}
				strcat(tmp, output);
				write(clients[index].sockfd, tmp, sizeof(tmp));
			}
			else if (strncmp(buffer, "chat", strlen("chat")) == 0)
			{
				printf("chat command received\n");
				// connect to particular client
				// check if the client and is not busy
				int client_id = atoi(buffer + strlen("chat "));
				if (client_id >= client_count)
					write(clients[index].sockfd, "Invalid client id\n", sizeof("Invalid client id\n"));
				else if (strcmp(clients[client_id].status, "busy") == 0)
					write(clients[index].sockfd, "Client is busy\n", sizeof("Client is busy\n"));
				else if (strcmp(clients[client_id].status, "dead") == 0)
					write(clients[index].sockfd, "Client is deleted\n", sizeof("Client is deleted\n"));
				else
				{
					char o1[1000], o2[1000];
					memset(o1, 0, 1000);
					memset(o2, 0, 1000);
					snprintf(o1, 1000 - 1, "client connected to client %d\n", index);
					snprintf(o2, 1000 - 1, "client connected to client %d\n", client_id);
					clients[index].dest_sockfd = clients[client_id].sockfd;
					strcpy(clients[client_id].status, "busy");
					strcpy(clients[index].status, "busy");
					clients[client_id].dest_sockfd = clients[index].sockfd;
					write(clients[index].sockfd, o2, sizeof(o2));
					write(clients[client_id].sockfd, o1, sizeof(o1));
				}
			}
			else
			{
			        
			        if (strcmp(buffer, "goodbye\n"))
			        {
				write(clients[index].sockfd, "Invalid command\n", sizeof("Invalid command\n"));
			}
			}
		}
		else if (strcmp(clients[index].status, "busy") == 0)
		{
			// client is busy and his message should be forwarded
			
			write(clients[index].dest_sockfd, buffer, n);
			if (strcmp(buffer, "goodbye\n") == 0)
			{
				strcpy(clients[index].status, "free");
			}
		}
		else
		{
			printf("Invalid status\n");
		}
	}
	strcpy(clients[index].status, "dead");
	pthread_exit(NULL);
}

int main(int argc,char *argv[])
{
if(argc < 2) //if the argc list is less than 2 it will print the statment.the argc should be equal to 2.
{
fprintf(stderr, "Port Number not provided. Program Terminated\n");
exit(1);
}
	int sockfd, n, tmp_n,server,portno;
	char buffer[1000];
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	 if(sockfd < 0)
    {
        perror("ERROR opening Socket.");
    }
     bzero((char *) &servaddr, sizeof(servaddr));
      portno = atoi(argv[1]);
       servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portno);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	if (listen(sockfd, 10) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("Server is currently running on port %d waiting for connections\n", portno);
	while (1)
	{
		int new_sockfd;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (new_sockfd == -1)
		{
			perror("accept");
			exit(1);
		}
		clients[client_count].index = client_count;
		clients[client_count].sockfd = new_sockfd;
		strcpy(clients[client_count].status, "free");
		client_count++;
		pthread_t thread;
		printf("New client connected from ip\n");
		pthread_create(&thread, NULL, thread_chat, &clients[client_count - 1].index);
	}
	return 0;
}
