/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/24/2019

Description:    This program  takes as a command line argument the IP Address and 
                port number of the server. 
                Inputs:
                v #: the user enters a “v”, a space, and then a version number. 
                    This version number is now used in all new messages.
                t # message string: the user enters a “t”, a space, and then a type
                    number, followed by the message.
                q: the user enters a “q” causes the socket to be closed and the program to terminate.
*/

#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
using namespace std;

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};
#define BUFFERSIZE sizeof( tcpMessage )

// this function starts in a thread to receive and display messages from the server
void recvServer( int sock )
{
    char buffer[BUFFERSIZE];
    string message, str;
    int type_;

    while ( true )
    {
        memset( buffer, 0, BUFFERSIZE );
        int n = recv( sock, buffer, BUFFERSIZE-1, 0 );
        buffer[n] = 0;
        
        // connection closed
        if ( n == 0 )
        {
            break;
        }
        if ( n < 0 )
        {
            perror("recv from client");
        }

        // the structure got unruly so using a formatted string here
        istringstream stream(buffer);
        stream >> type_;
        string temp;
        message = "";
        while ( stream >> temp )
        {
            message += temp + " ";
        }

        cout << "Received msg type: " << type_ << "; Msg: " << message;
    }

    close( sock );
}

// the main function for creating the socket and processing user commands
int main( int argc, char *argv[] )
{
    string input_, message, tcp_message_str;
    char command;
    int version_, type_, sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFERSIZE];

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(argv[1]);
    memset( (char *)&serv_addr, sizeof(serv_addr), 0 );
    serv_addr.sin_family = AF_INET;
    memmove( (char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // start thread for receiving and detach
    thread t( recvServer, sockfd );
    t.detach();
    
    while( true )
    {
        cout << "\nPlease enter command: ";
        getline( cin, input_ );
        istringstream stream(input_);
        stream >> command;
        if ( command == 'q' )
        {
            cout << "Terminating." << "\n";
            break;
        }
        if ( command == 'v' )
        {
            stream >> version_;
        }
        if ( command == 't' )
        {
            stream >> type_;
            string temp;
            message = "";
            while ( stream >> temp )
            {
                message += temp + " ";
            }
            // // create struct
            // // got unruly while trying to serialize
            // struct tcpMessage sendMsg;
            // sendMsg.nVersion = version_;
            // sendMsg.nType = type_;
            // sendMsg.nMsgLen = sizeof(message);
            // strcpy( sendMsg.chMsg, message.c_str() );
            // memcpy( buffer, &sendMsg, sizeof(tcpMessage) );
            // struct tcpMessage deser;
            // memcpy( &deser, buffer, sizeof(deser));
            // cout << "sending buffer: " << deser.chMsg;

            // create a formatted string instead of the structure
            tcp_message_str = to_string(version_) + " " + to_string(type_) + " " + message;
            strcpy( buffer, tcp_message_str.c_str() );

            // send
            int n = send(sockfd, buffer, strlen(buffer), 0);
            memset( buffer, 0, BUFFERSIZE );
        }
    }

    close( sockfd );

    return 0;
}