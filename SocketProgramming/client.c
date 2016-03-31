#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DATA "Hello World of Socket"

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	char buff[1024];

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("Socket failed");
		exit(1);
	}
	
	server.sin_family=AF_INET;
	hp=gethostbyname(argv[1]);
	if(hp == 0)
	{
		perror("gethostname failed");
		close(sock);
		exit(1);
	}	

	memcpy (&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(5000);

	if(connect(sock,(struct sockaddr *)&server, sizeof(server))< 0)
	{
		perror("Connect failed");
		close(sock);
		exit(1);
	}

	if(send(sock, DATA, sizeof(DATA),0) < 0)
	{
		perror ("Send failed");
		close(sock);
		exit(1);
	}

printf("Sent %s \n", DATA);
close(sock);

return 0;
}


