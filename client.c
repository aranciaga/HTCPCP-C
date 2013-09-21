#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
	if (!argv[1] | !argv[2]){
		printf("Usage: ./client <IP> <PORT>\n");
		exit(1);
	}
	
	int sckt, check;
	struct sockaddr_in server;
	char send_message[2000], recv_message[2000];
	
	int connected = 1;

	sckt = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sckt==-1) printf("Socket cannot be created");

	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));

	if(connect(sckt, (struct sockaddr *)&server, sizeof(server))<0) perror("Connection failed");
	printf("You are connected! have fun\n");
	while(1){
		printf("HTCPCP>");
		scanf("%s", send_message);
		if (send(sckt, send_message, strlen(send_message), 0)<0) puts("Fail (send)");
		if (recv(sckt, recv_message, 2000,0)<0) puts("Fail (recv)");
		puts(recv_message);

	}

	close(sckt);
	return 0;
}