#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


#define ECHOMAX 255U /* Longest string to echo */

#define TIMEOUT_SECS 2 /* Seconds between retransmits */
#define MAXTRIES 5 /* Tries before giving up */

int tries=0; /* Count of times sent - GLOBAL for signal handler access */

void DieWithError(char *errorMessage); /* Error handling function */
void CatchAlarm(int ignored); /* Handler for SIGALRM */

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr; /* Source address of echo */
    unsigned short echoServPort; /* Echo server port */
    unsigned int fromSize; /* Buffer for echo string */
    struct sigaction myAction; /* String to send to echo server */
    char *servIP; /* IP address of server */
    char *echoString; /* For setting signal handler */
    char echoBuffer[ ECHOMAX + 1 ]; /* In-out of address size for recvfrom() */
    ssize_t echoStringLen; /* Length of string to echo */
    ssize_t respStringLen; /* Size of received datagram */
    
    
    
    
    
    
    
    
    
    if( ( argc < 3 ) || ( argc > 4 ) ) /* Test for correct number of arguments */
    {
        fprintf( stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[ 0 ] );
        exit( 1 );
    }

    servIP = argv[ 1 ]; /* First arg: server IP address (dotted quad) */
    echoString = argv[ 2 ]; /* Second arg: string to echo */
    
    if( ( echoStringLen = (ssize_t)strlen( echoString ) ) > ECHOMAX )
        DieWithError( "Echo word too long" );

    if( argc == 4 )
        echoServPort = (unsigned short)atoi( argv[ 3 ] ); /* Use given port, if any */
    else
        echoServPort = 7; /* 7 is well-known port for echo service */
    
    /* Create a best-effort datagram socket using UDP */
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( " socket () failed" );

    /* Set signal handler for alarm signal */
    myAction.sa_handler = CatchAlarm;

    if( sigfillset( &myAction.sa_mask) < 0 ) /* block everything in handler */
        DieWithError( "sigfillset () failed" );

    myAction.sa_flags = 0;
    if( sigaction( SIGALRM, &myAction, 0 ) < 0 )
        DieWithError("sigaction() failed for SIGALM") ;
    
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr( servIP );
    echoServAddr.sin_port = htons(echoServPort); /* Server port */
    
    /* Send the string to the server */
    if( sendto( sock, echoString, (size_t)echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof( echoServAddr ) ) != echoStringLen )
        DieWithError( "sendto() sent a different number of bytes than expected" );

    /* Get a response */
    fromSize = sizeof( fromAddr );
    alarm( TIMEOUT_SECS ); /* Set the timeout */
    while( ( respStringLen = recvfrom( sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize ) ) < 0 )
    {
        if( errno == EINTR ) /* Alarm went off */
        {
            if( tries < MAXTRIES ) /* incremented by signal handler */
            {
                printf( "timed out, %d more tries...\n", MAXTRIES - tries );
                if( sendto( sock, echoString, (size_t)echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof( echoServAddr ) ) != echoStringLen )
                    DieWithError("sendto() failed");
                alarm( TIMEOUT_SECS );
            }
            else
            {
                DieWithError( "No Response" );
            }
        }
        else
        {
            DieWithError("recvfrom() failed");
        }
    }    
    
    /* recvfrom() got something-- cancel the timeout */
    alarm( 0 ) ;
    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0' ;
    printf( "Received' %s\n", echoBuffer ); /* Print the received data */

    close( sock );
    exit( 0 );
}

void CatchAlarm( int ignored )
{
    tries += 1;
}

void DieWithError(char *errorMessage)
{
    perror( errorMessage );
    exit( 1 );
}


