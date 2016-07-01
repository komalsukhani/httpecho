#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<err.h>
#include<string.h>

#define SIZE 99999

using namespace std;

void requestHandler(int);
void reverse(char *);

int main(int argc, char* argv[])
{
    int optval=1, client_fd,sock,port;
    struct sockaddr_in server_addr, cli_addr;
    socklen_t sin_len=sizeof(cli_addr);

    // Ensure correct number of arguements are provided
    if(argc<3)
    {
        cout<<"Usage: file IP_Address Port";
        return 0;
    }

    // Create Socket
    sock=socket(AF_INET, SOCK_STREAM, 0);
    if(sock<0)
        err(1,"can't open socket");
    port=atoi(argv[2]);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    inet_pton(AF_INET,argv[1],&(server_addr.sin_addr));

    // Bind socket to given IP addr and port number
    if(bind(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
    {
        close(sock);
        err(1, "Can't bind");
    }

    // Listen for incoming connections
    listen(sock, 10);
    while(1)
    {
        // Accept a connection
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        if (client_fd == -1)
        {
            perror("Can't accept");
            continue;
        }
        else
        {
            // Fork new process for each accepted request
            if(fork()==0)
            {
                close(sock);
                //Serve request
                requestHandler(client_fd);
                //Close connection
                if(close(client_fd)==0)
                {
                    exit(0);
                }
                else
                    perror("Filed to close connection");
            }
            close(client_fd);
        }
    }
}
// Read data from request body and return reversed string as response
void requestHandler(int n)
{
    char response_header[]="HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n\r\n";
    char message[SIZE];
    int rcvd, fd,len,count=0,low,upper;
    char *path;
    int i;

    memset( (void*)message, (int)'\0', SIZE );
    rcvd=recv(n, message, SIZE, 0);
    if (rcvd<0)
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)
        fprintf(stderr,"Client disconnected unexpectedly.\n");
    else
    {
				// Get request header
        for(i=0; !(*(message+i)=='\r' && *(message+i+1)=='\n'); i++);
        char *request_header=new char[i+2];
        strncpy(request_header,message,i);
        request_header[i+1]='\0';
        strtok(request_header," \t\n");
        path=strtok(NULL," \t\n");

				// Check if request is sent on correct path
        if(strcmp(path,"/reverse"))
        {
            char response[]="HTTP/1.1 404  Not Found";
            write(n,response,strlen(response));
						printf("%s\n",response);
            return;
        }

        // Read data from request body
        char *f=strstr(message,"\r\n\r\n");

        if(f!=NULL)
        {
            f+=4;
            len=strlen(message)-(f-message);
            char *data=new char[len];
            strncpy(data,f,len);

            // Reverse data
            reverse(data);

            // Return reversed data to client
            write(n,response_header,strlen(response_header));
						printf("%s",response_header);
            strcat(data,"\r\n");
            write(n,data,strlen(data));
						printf("%s\n",data);
        }
    }

}
// Reverse string
void reverse(char * str)
{
    char temp;
    int i,len;
    len=strlen(str);
    for(i=0; i<len/2; i++)
    {
        temp=str[i];
        str[i]=str[len-i-1];
        str[len-i-1]=temp;
    }
}
