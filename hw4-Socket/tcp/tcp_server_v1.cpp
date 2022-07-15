#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <iostream> /* Needed for close() */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
using namespace std;

#define BACKLOG 10

void conn( int sock )
{
    int n;
    char buffer[256];

    // struct sockaddr_in* p = (struct sockaddr_in *)&addr;
    // struct in_addr ip = p->sin_addr;
    // char str[INET_ADDRSTRLEN];
    // inet_ntop( AF_INET, &ip, str, INET_ADDRSTRLEN );
    // cout << str;

    memset( buffer, 0, 256 );
    n = recv( sock, buffer, 255, 0 );
    buffer[n] = 0;
    cout << buffer << "\n";
}

int main( int argc, char *argv[] )
{
    int sockfd, newfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    memset( (char *)&serv_addr, 0, sizeof( serv_addr ) );
    portno = atoi( argv[1] );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );

    bind( sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) );

    listen( sockfd, BACKLOG );

    while( true ) 
    {
        int newfd = accept( sockfd, (struct sockaddr *)&cli_addr, &clilen );
        thread t( conn, newfd );
        t.detach();
    }

    shutdown( sockfd, SHUT_RDWR );
    close( sockfd );
    shutdown( newfd, SHUT_RDWR );
    close( newfd );

    return 0;
}
