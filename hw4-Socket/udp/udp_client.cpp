/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/24/2019

Description:    This program  takes as a command line argument the IP Address and 
                port number of the server. 
                Inputs:
                v #: the user enters a “v”, a space, and then a version number. 
                    This version number is now used in all new messages.
                t # # message string: the user enters a “t”, a space, and then a type
                    number, and then a sequence number, followed by the message (if any).
                q: the user enters a “q” causes the socket to be closed and the program to terminate.
*/

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <thread>
#include <unistd.h>
using namespace std;

struct udpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    unsigned long lSeqNum;
    char chMsg[1000];
};
#define BUFFERSIZE sizeof(udpMessage) // max size of sent/received stream

// this functions start in a thread to receive and display boradcasts from the server
void startReceiving( int portno, int &sockfd )
{
    char buffer[BUFFERSIZE];
    struct sockaddr from;
    socklen_t fromlen = sizeof(struct sockaddr_in);

    // loop for receiving messages
    while( true ) 
    {
        int n = recvfrom(sockfd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&from, &fromlen);

        // closed connection
        if ( n == 0 )
        {
            break;
        }

        buffer[n] = 0;
        cout << "Received Msg: " << buffer;
    }

    close( sockfd );
}

// the main function for creating socket, processing user commands and sending messages to the server
int main(int argc, char *argv[])
{
    int sockfd, portno, n, version_, type_, seq_;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    string udp_message_str, input_, message;
    char command;

    socklen_t fromlen = 0;
    struct sockaddr  from;
    char buffer[1024];
    char broadcast = '1';
    
    portno = atoi(argv[2]);

    memset((char *)&from, 0, sizeof(sockaddr));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // enable receiving broadcasting
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    
    server = gethostbyname(argv[1]);
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // create a thread for receiving and detach
    thread t( startReceiving, portno, ref(sockfd) );
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
            stream >> seq_;
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

            udp_message_str = to_string(version_) + " " + to_string(type_) + " " + to_string(seq_) + " " + message;
            strcpy( buffer, udp_message_str.c_str() );

            n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            memset( buffer, 0, BUFFERSIZE );

            // closed connection
            if ( n == 0 )
            {
                break;
            }
        }
    }

    close(sockfd);
    return 0;
}