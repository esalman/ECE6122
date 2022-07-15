/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/24/2019

Description:    This program takes as a takes as a command line argument the port 
                number on which the UDP Server will receive messages. 
                The UDP server collects parts of a larger composite message from 
                the clients and will broadcast the composite message to all clients
                when commanded.
                Inputs:
                0:  the server immediately sends to all clients the current composite message and clears out the composite message.
                1:  the server immediately clears out the composite message.
                q:  the server immediately displays to the console the composite message but takes no other actions.
                Functionality:
                * If nType == 0 the composite message is immediately cleared and anything in the chMsg buffer is ignored.
                * If nType == 1 the composite message is immediately cleared and the message in chMsg is used as the start of a new composite message
                * If nType == 2 the message in chMsg is added to the composite message based on its lSeqNum
                * If nType == 3 the server immediately sends to all clients the current composite message and clears out the composite message.
*/

#include <arpa/inet.h>
#include <cerrno>
#include <iostream>
#include <map>          // for maintaining ordered list of messages
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

struct udpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    unsigned long lSeqNum;
    char chMsg[1000];
};
#define BUFFERSIZE sizeof(udpMessage)   // max size of sent/received stream
map<int, string> compositeMessage;      // for storing the composite message
socklen_t fromlen = sizeof(struct sockaddr);

// a vector for storing client addresses
vector< struct sockaddr > clients;

// this function loops over the map and prints out the composite message sequentially
void printCompositeMessage()
{
    string message;

    cout << "Composite Msg: ";
    for ( map<int, string>::iterator it = compositeMessage.begin(); it != compositeMessage.end(); ++it )
    {
        message += it->second;
    }
    cout << message << "\n";
}

// this function generates the composite message from the map and broadcasts to all clients
void sendCompositeMessage( int portno, int sockfd )
{
    char buffer[BUFFERSIZE];
    string message, to_send;
    
    // generate the composite message
    for ( map<int, string>::iterator it = compositeMessage.begin(); it != compositeMessage.end(); ++it )
    {
        message += it->second;
    }

    // handle message.length > 1000
    if ( message.length() > 1000 )
    {
        to_send = message.substr( 0, 1000 );
        // create new message 
        message = message.substr( 1000, message.length()-1000 );
    }
    else
    {
        to_send = message;
    }

    // copy message to buffer
    strcpy( buffer, to_send.c_str() );

    // loop over the clients and broadcast
    for ( vector< struct sockaddr >::iterator it = clients.begin(); it != clients.end(); ++it )
    {
        int n = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&(*it), fromlen);
    } 
}

// this function is started in a new thread to receive incoming messages
void startReceiving( int portno, int &sockfd )
{
    int n;
    char buffer[BUFFERSIZE];
    int version_, type_, seq_;
    string message;
    
    // loop for receiving messages
    while( true ) 
    {
        // create sockaddr for a client 
        struct sockaddr from;
        n = recvfrom(sockfd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&from, &fromlen);
        // store the client
        clients.push_back( from );
        buffer[n] = 0;
        
        // handle received message
        // it got unruly trying to serialize the udpMessage into the buffer so I am just parsing a formatted string
        istringstream stream(buffer);
        // grab version, sequence and type from the stream buffer
        stream >> version_;
        stream >> type_;
        stream >> seq_;
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
        
        // implement different functionality based on type
        if ( type_ == 0 )
        {
            compositeMessage.clear();
            continue;
        }
        
        if ( type_ == 1 )
        {
            compositeMessage.clear();
            compositeMessage[0] = message;
            continue;
        }

        if ( type_ == 2 )
        {
            compositeMessage[seq_] = message;
            continue;
        }

        if ( type_ == 3 )
        {
            sendCompositeMessage( portno, sockfd );
            compositeMessage.clear();
            continue;
        }
    }

    close( sockfd );
}

// the main function for creating the UDP server socket and processing user commands
int main( int argc, char *argv[] )
{
    char command;
    int portno, sockfd;
    struct sockaddr_in serv_addr;
    char broadcast = '1';
    
    portno = atoi( argv[1] );

    // create listening socket
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

    // enable broadcasting
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    // bind to socket and start listening
    memset( (char *)&serv_addr, 0, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    bind( sockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) );

    // create a thread for receiving and detach
    thread t( startReceiving, portno, ref(sockfd) );
    t.detach();

    // command loop
    while( true )
    {
        cout << "\nPlease enter command: ";
        cin >> command;
        if ( command == '0' )
        {
            sendCompositeMessage( portno, sockfd );
            compositeMessage.clear();
        }
        else if ( command == '1' )
        {
            compositeMessage.clear();
        }
        else if ( command == '2' )
        {
            printCompositeMessage();
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