#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>

#define TRUE 1
#define FALSE 0
#define PORT 5555

int main(int argc, char *argv[])
{
	/*Variables*/
	int opt = TRUE;
	int master_socket, addrlen, new_socket,client_socket[30], max_clients = 30, ret, i , valread, sd;
	int max_sd;
	struct sockaddr_in server;

	char buff[1025];

	fd_set read_set,exc_set;

	char *message = "Got the message";

	for( i =0; i< max_clients; i++)
	{
		client_socket[i]=0;
	}
	

	/*Create socket*/
	if((master_socket=socket(AF_INET,SOCK_STREAM,0))==0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
	
	if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(PORT);


	/*Call bind*/
	if(bind(master_socket,(struct sockaddr *)&server, sizeof(server))<0)
	{
		perror("Bind Failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n",PORT);	

	/*Listen*/
	if(listen(master_socket,3)<0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	addrlen=sizeof(server);
	puts("Waiting connections...");

	/*Accept*/
	while(TRUE)
	{
		FD_ZERO(&read_set);
		FD_SET(master_socket,&read_set);
		max_sd=master_socket;

		for(i=0;i<max_clients;i++)
		{	
			sd=client_socket[i];
			if(sd>0)
				FD_SET(sd, &read_set);
			if(sd>max_sd)
				max_sd=sd;
		}

		ret=select(max_sd+1, &read_set, NULL, NULL,NULL);
		if((ret < 0)&&(errno!=EINTR))
		{
			printf("select error");
		}

		if(FD_ISSET(master_socket,&read_set))
		{
			if((new_socket=accept(master_socket,(struct sockaddr *)&server, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			printf("New connection, socket fd is %d, ip is %s, port is %d \n",new_socket, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
			
			if(send(new_socket, message, strlen(message),0) != strlen(message))
			{	
				perror("Send");
			}
			puts("Welcome message sent sucessfully");

			for(i=0;i<max_clients;i++)
			{
				if(client_socket[i]==0)
				{
					client_socket[i]=new_socket;
					printf("Adding to list of sockets as %d\n",i);
					break;
				}
			}
		}

		for(i=0;i<max_clients;i++)
		{
			sd=client_socket[i];
			if(FD_ISSET(sd, &read_set))
			{
				if((valread = read(sd,buff,1024))==0)
				{
					getpeername(sd,(struct sockaddr*)&server, (socklen_t*)&addrlen);
					printf("Host disconnected, ip %s, port %d \n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
					close(sd);
					client_socket[i]=0;
				}
				else
				{
					buff[valread]='\0';
					send(sd, buff, strlen(buff),0);
				}
			}
		}

	}
return 0;
}
