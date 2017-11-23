#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAXPENDING 5 /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket); /* TCP client handling function */
int CreateTCPServerSocket(unsigned short port); /* Create TCP server socket */
int AcceptTCPConnection(int servSock); /* Accept TCP connection request */


int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    unsigned short echoServPort;
    pid_t processID;
    unsigned int childProcCount = 0; /* Number of child processes */

    if( argc != 2 ) /* Test for correct number of arguments */
    {
        fprintf( stderr, "Usage: %s <Server Port>\n", argv[ 0 ] );
        exit( 1 );
    }

    echoServPort = (unsigned short)atoi( argv[ 1 ] ); /* First arg: local port */
    servSock = CreateTCPServerSocket( echoServPort );
    for( ;; ) /* Run forever */
    {
        clntSock = AcceptTCPConnection( servSock );
        /* Fork child process and report any errors */
        if( ( processID = fork() ) < 0 )
        {
            DieWithError( "fork() failed" );
        }
        else if( processID == 0 ) /* This is the child process */
        {
            close( servSock ); /* Child closes listening socket */
            HandleTCPClient( clntSock );
            exit( 0 );
        }

        /* Child process terminates */
        printf( "with child process: %d\n", (int)processID );
        close( clntSock ); /* Parent closes child socket descriptor */
        childProcCount++; /* Increment number of outstanding child processes */
        while( childProcCount ) /* Clean up all zombies */
        {
            processID = waitpid( (pid_t)-1, NULL, WNOHANG ); /* Nonblocking wait */
            if( processID < 0 ) /* waitpid() error? */
                DieWithError( "waitpid() failed" );
            else if( processID == 0 ) /* No zombie to wait on */
                break;
            else
                childProcCount--; /* Cleaned up after a child */
        }
    }
    /* NOT REACHED */

}


int CreateTCPServerSocket(unsigned short port)
{
    int sock; /* Socket to create */
    struct sockaddr_in echoServAddr; /* Local address */
    
    /* Create socket for incoming connections */
    if( ( sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
        DieWithError( " socket () failed" );

    /* Construct local address structure */
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY ); /* Any incoming interface */
    echoServAddr.sin_port = htons( port ); /* Local port */

    /* Bind to the local address */
    if( bind( sock, (struct sockaddr *)&echoServAddr, sizeof( echoServAddr ) ) < 0 )
        DieWithError( "bind () failed" );

    /* Mark the socket so it will listen for incoming connections */
    if( listen( sock, MAXPENDING ) < 0 )
        DieWithError( "listen() failed" ) ;
    return sock;

}


int AcceptTCPConnection(int servSock)
{
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen; /* Length of client address data structure */
    
    /* Set the size of the in-out parameter */
    clntLen = sizeof( echoClntAddr );
 
    /* Wait for a client to connect */
    if( ( clntSock = accept( servSock, (struct sockaddr *) &echoClntAddr, &clntLen ) ) < 0 )
        DieWithError( "accept () failed" );

    /* clntSock is connected to a client! */
    printf( "Handling client %s\n", inet_ntoa( echoClntAddr.sin_addr ) );
    return clntSock;
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

