/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/24/2019

Description:    This program takes as a takes as a command line argument the port 
                number on which the TCP Server will listen for connection requests. 
                The TCP server maintains a list of all connected clients and is  able
                to receive data from clients without blocking the main application thread.
                Inputs:
                0:  prints to the console the last message received (if any)
                1:  prints to the console a list of all connected clients (ip address and port number)
                q:  closes all sockets and terminates the program
                Functionality:
                * If `nVersion` is not equal to 1 then the incoming message is ignored
                * If `nType == 0` then the message should be sent to all other connected clients 
                * If nType == 1 then the message is reversed and sent back to only the client that sent the message
*/

#include <algorithm>    // for reverse
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sstream>      // for parsing received stream
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

#define BACKLOG 20      // how many connections to accept 
// tcp message structure
struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};
#define BUFFERSIZE sizeof( tcpMessage ) // max size of sent/received stream
char lastMessage[1000];

// this structure contains the connected client sockets and sockaddr_in
struct clientStruct {
    struct sockaddr_in* cli;
    int sock;
};
// a vector of connected clients, formatted as the above structure
vector< struct clientStruct > clients;

// this function is started in a new thread for each new connection from a client
void connClient( int sock )
{
    char buffer[BUFFERSIZE];
    string message, str;
    int version_, type_;

    // loop for communicating with the connected client
    while ( true )
    {
        // clear the buffer 
        memset( buffer, 0, BUFFERSIZE );
        // blocking receive
        int n = recv( sock, buffer, BUFFERSIZE-1, 0 );
        buffer[n] = 0;
        
        // handle connection closed
        if ( n == 0 )
        {
            // loop over vector of connected clients
            for ( vector< struct clientStruct >::iterator it = clients.begin(); it != clients.end(); ++it )
            {
                if ( (*it).sock == sock )
                {
                    clients.erase( it );
                    break;
                }
            }
            break;
        }
        if ( n < 0 )
        {
            // handle recv error
            perror("recv from client");
        }

        // handle received message
        // it got unruly trying to serialize the tcpMessage into the buffer so I am just parsing a formatted string
        istringstream stream(buffer);
        // grab version and type from the stream buffer
        stream >> version_;
        stream >> type_;
        // grab the rest of the message
        string temp;
        message = "";
        while ( stream >> temp )
        {
            message += temp + " ";
        }
        
        // ignore if version != 1
        if ( version_ != 1 )
        {
            continue;
        }

        // save last message
        strcpy( lastMessage, message.c_str() );

        // type==0: send to all other clients 
        if ( type_ == 0 )
        {
            // loop over the connected clients
            for ( vector< struct clientStruct >::iterator it = clients.begin(); it != clients.end(); ++it )
            {
                // skip the sender
                if ( (*it).sock == sock )
                {
                    continue;
                }
                // prepend message with type
                message = to_string(type_) + " " + lastMessage;
                memset( buffer, 0, BUFFERSIZE );
                strcpy( buffer, message.c_str() );
                n = send((*it).sock, buffer, strlen(buffer), 0);
            }
        }
        // type==1: reverse incoming message and send to the client
        else if ( type_ == 1 )
        {
            // algorithm::reverse
            reverse( message.begin(), message.end() );
            // prepend message with type
            message = to_string(type_) + " " + message;
            memset( buffer, 0, BUFFERSIZE );
            strcpy( buffer, message.c_str() );
            n = send(sock, buffer, strlen(buffer), 0);
        }
    }

    // close socket
    close( sock );
}

// create a socket and start listening; start a new thread for every new connection
// input: port number to listen to
void startListening( int portno, int &sockfd )
{
    int newfd, n;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    
    // create listening socket
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    // bind to socket and start listening
    memset( (char *)&serv_addr, 0, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    bind( sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) );
    listen( sockfd, BACKLOG );

    // loop for accepting new connections
    while( true ) 
    {
        // new socket 
        int newfd = accept( sockfd, (struct sockaddr *)&cli_addr, &clilen );
        
        // store client socket and sockaddr_in in a vector of clientStruct structures
        struct sockaddr_in* p = (struct sockaddr_in *)&cli_addr;
        struct clientStruct c;
        c.cli = p;
        c.sock = newfd;
        clients.push_back( c );

        // start new thread for the client and detach
        thread t( connClient, newfd );
        t.detach();
    }
}

// this function lists connected clients IP addresses and ports
void listClients()
{
    char str[INET_ADDRSTRLEN];

    cout << "Number of clients: " << clients.size() << "\n";
    cout << "IP Address\tPort\n";

    for ( vector< struct clientStruct >::iterator it = clients.begin(); it != clients.end(); ++it )
    {
        struct in_addr ip = (*it).cli->sin_addr;
        inet_ntop( AF_INET, &ip, str, INET_ADDRSTRLEN );
        cout << str << "\t" << ntohs((*it).cli->sin_port) << "\n";
    }
}

// The main function takes as a command line argument the port 
// number on which the TCP Server will listen for connection requests.
int main( int argc, char *argv[] )
{
    char command;
    int portno, sockfd;
    
    portno = atoi( argv[1] );

    // create a thread for listening and detach
    thread t( startListening, portno, ref(sockfd) );
    t.detach();

    // command loop
    while( true )
    {
        cout << "\nPlease enter command: ";
        cin >> command;
        if ( command == '0' )
        {
            cout << "Last message: " << lastMessage << "\n";
        }
        else if ( command == '1' )
        {
            listClients();
        }
        else if ( command == 'q' )
        {
            close( sockfd );
            cout << "Terminating." << "\n";
            break;
        }
    }
    return 0;
}
