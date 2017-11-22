#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5

void DieWithError( char *errorMessage );
void HandleTCPClient( int clntSocket );

/* for atoi() */
/* for memset() */
/* for close() */
/* Maximum outstanding connection requests */
int main( int argc, char *argv[] )
{
    /* Error handling function */
    /* TCP client handling function */
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */
    
    if( argc != 2 ) /* Test for correct number of arguments */
    {
        fprintf( stderr, "Usage: %s <Server Port>\n", argv[ 0 ] ) ;
        exit( 1 );
    }
    echoServPort = (unsigned short)atoi( argv[ 1 ] ); /* First arg: local port */
    
    /* Create socket for incoming connections */
    if( ( servSock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
        DieWithError( "socket () failed" ) ;
    
    /* Construct local address structure */
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY ); /* Any incoming interface */
    echoServAddr.sin_port = htons( echoServPort ); /* Local port */

    /* Bind to the local address */
    if( bind( servSock, (struct sockaddr *)&echoServAddr, sizeof( echoServAddr ) ) < 0 )
        DieWithError( "bind () failed" );

    /* Mark the socket so it will listen for incoming connections */
    if( listen( servSock, MAXPENDING ) < 0 )
        DieWithError( "listen() failed");

    for( ;; ) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof( echoClntAddr );
  
        /* Wait for a client to connect */
        if( ( clntSock = accept( servSock, (struct sockaddr *) &echoClntAddr, &clntLen ) ) < 0 )
            DieWithError( "accept() failed" );

        /* clntSock is connected to a client! */
        printf( "Handling client %s\n", inet_ntoa( echoClntAddr.sin_addr ) );
        HandleTCPClient( clntSock );
    }
    /* NOT REACHED */

}


void DieWithError(char *errorMessage)
{
    perror( errorMessage );
    exit( 1 );
}

#define RCVBUFSIZE 32 /* Size of receive buffer */


void HandleTCPClient(int clntSocket)
{

    /* Error handling function */
    char echoBuffer[ RCVBUFSIZE ];
    ssize_t recvMsgSize;

    /* Buffer for echo string */
    /* Size of received message */
    /* Receive message from client */
    if( ( recvMsgSize = recv( clntSocket, echoBuffer, RCVBUFSIZE, 0 ) ) < 0 )
        DieWithError( "recv() failed" );

    /* Send received string and receive again until end of transmission */
    while( recvMsgSize > 0 ) /* zero indicates end of transmission */ 
    {
        /* Echo message back to client */
        if( send( clntSocket, echoBuffer, (size_t)recvMsgSize, 0 ) != recvMsgSize )
            DieWithError( "send() failed" );

        /* See if there is more data to receive */
        if( ( recvMsgSize = recv( clntSocket, echoBuffer, RCVBUFSIZE, 0 ) ) < 0 )
            DieWithError( "recv() failed" ) ;
    }    
    close( clntSocket ); /* Close client socket */
}
