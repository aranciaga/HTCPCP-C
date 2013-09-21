#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

#define portConnection 8081
#define useSerialConn 0 //change it to 1 if you want to connect this program to serial connection

char* serialPort = "ttyACM0";
char* str;

int ids[] = {14, 29}; // 14 = BREW ; 29 = WHEN

const char* logo[] = {
"          {",
"       }   }   {",
"      {   {  }  }",
"       }   }{  {",
"      {  }{  }  }",
"     ( }{ }{  { )",
"    .-{   }   }-.",
"   ( ( } { } { } )",
"   |`-.._____..-'|",
"   |             |;--.		HTCPCP implementation",
"   |     	 (_   |	   		by",
"   |             | )  )		      Rainbow",
"   |             |/  /",
"   |             /  /",
"   |            (  /",
"   |              /",
"   |             |",
"    `-.._____..-'",
"",
};

int BREW = 0;
int teapot = 0;


void handleSerialConnection(int signal){
    if (useSerialConn>0){
        FILE *open;
        if (open = fopen(serialPort, "w")){
            fprintf(open, "%s", signal);
            fclose(open);
        } else {
            printf("%s could not be opened", serialPort);
        }
    }  
}

int handleCommands(char* command, int client_sock, char* client_message){
    char* message;
    if(teapot==0){
        if (strstr(client_message, "GET")){
            if (BREW>0){
                message="Making normal coffee\n";
                write(client_sock, message, strlen(message)*2);
            } else {
                message = "Idle\n";
                write(client_sock, message, strlen(message)*2);
            }
        } else if(strstr(client_message, "BREW")){
            if(BREW==0){
                BREW=1;
                message = "Starting...\n";
                write(client_sock, message, strlen(message)*2);
                handleSerialConnection(14);
            }
            message = "           ";
            write(client_sock, message, strlen(message)*2);
        } else if(strstr(client_message, "WHEN")){
            if(BREW>0){
                BREW=0;
                message = "Stopping...\n";
                write(client_sock, message, strlen(message)*2);
                handleSerialConnection(29);
            }
            message = "             ";
            write(client_sock, message, strlen(message)*2);
        } else {
            message = "406 Not Acceptable\n";
            write(client_sock, message, strlen(message)*2);
        }
    } 
    message = "418 I'm a teapot\n";
    write(client_sock, message, sizeof(message)*2);

    return 0;
}

void *Conn(void *socket_desc){
    int client_sock = *(int*)socket_desc;
    int read_size;
    char client_message[2000];
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
            handleCommands(client_message, client_sock, client_message);
    }
         
    if(read_size == 0)
        {
            printf("Client disconnected\n");
            fflush(stdout);
        }
    else if(read_size == -1)
        {
            perror("recv failed");
        }
    }


int main(void)
{
    int socket_desc , client_sock , c , read_size, *new_sock;
    struct sockaddr_in server , client;
    char client_message[2000];
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    for (int i=0; i<18; i++){
    	printf("%s\n", logo[i]);
    }

    if (socket_desc!=-1){
		printf("\n[OK] Socket\n");
    } else {
    	printf("[Fail] Socket\n");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portConnection);
     
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("[Fail] Bind\n");
        return 1;
    }
    printf("[OK] Bind\n");
     
    listen(socket_desc , 3);
     
    puts("Listening");
    c = sizeof(struct sockaddr_in);
    

    while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)){

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  Conn , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        printf(">>%s might be interested in some coffee\n", inet_ntoa(client.sin_addr));
    }
}   