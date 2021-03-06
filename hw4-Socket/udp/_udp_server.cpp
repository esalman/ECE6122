/* 
   http://www.linuxhowtos.org/C_C++/socket.htm
   A simple server in the internet domain using TCP
   The port number is passed as an argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <iostream>

#ifdef _WIN32
   /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501  /* Windows XP. */
    #endif
    #include <winsock2.h>
    #include <Ws2tcpip.h>

    #pragma comment (lib, "Ws2_32.lib")
#else
   /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */

    typedef int SOCKET;
#endif
/////////////////////////////////////////////////
// Cross-platform socket initialize
int sockInit(void)
{
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
    return 0;
#endif
}
/////////////////////////////////////////////////
// Cross-platform socket quit
int sockQuit(void)
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}
/////////////////////////////////////////////////
// Cross-platform socket close
int sockClose(SOCKET sock)
{

    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) 
    {
        status = closesocket(sock); 
    }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) 
    { 
        status = close(sock);
    }
#endif

    return status;

}
/////////////////////////////////////////////////
// Output error message and exit
void error(const char *msg)
{
    perror(msg);
    // Make sure any open sockets are closed before calling exit
    exit(1);
}
/////////////////////////////////////////////////
// Main
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t fromlen;
    char buffer[1024];
    struct sockaddr_in serv_addr, from;
    int n;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockInit();
    // Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Make sure the socket was created
    if (sockfd < 0)
        error("ERROR opening socket");
    // Zero out the variable serv_addr
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    // Convert the port number string to an int
    portno = atoi(argv[1]);
    
    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);
    // Bind the socket to the port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }
    printf("Waiting on messages...\n");

    fromlen = sizeof(struct sockaddr_in);
    while (true)
    {
        n = recvfrom(sockfd, buffer, 1023, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
            error("recvfrom");
        buffer[n] = 0;  // Null terminate
        printf("Received a datagram: %s", buffer);
        n = sendto(sockfd, "Got your message\n", 17, 0, (struct sockaddr *)&from, fromlen);
        if (n < 0)
            error("sendto");
    }

    sockClose(newsockfd);
    sockClose(sockfd);

    sockQuit();

#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}