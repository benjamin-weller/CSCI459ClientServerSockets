#include "server.h"

/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h> //for IOs
#include <string.h>
#include <unistd.h>
#include <sys/types.h> //for system calls
#include <sys/socket.h> //for sockets
#include <netinet/in.h> //for internet
#include <pthread.h> //for thread

void threadFunction(int newSocketFileDescriptor)
{
    while (true)
    {
        bzero(buffer, 256);
        n = read(newSocketFileDescriptor, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");

        if (strcomp(buffer, "EXIT"))
        {
            close(newSocketFileDescriptor);
            return;
        }

        //Else go on as usual.
        printf("Here is the message: %s\n", buffer);

        n = write(newSocketFileDescriptor, "I got your message", 18);
        if (n < 0) error("ERROR writing to socket");
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
/*a funciton to print out error message and then abort */
int main(int argc, char *argv[])
{
    int sockfd, newSocketFileDescriptor, portno;
/*
sockfd and newsockfd are file descriptors, i.e. array subscripts into the file
descriptor table.
These two variables store the values returned by the socket system call and the
accept system call.
portno stores the port number on which the server accepts connections.
*/
    socklen_t clilen; /*clilen stores the size of the address of the client. This is
needed for the accept system call.*/
    char buffer[256]; /*The server reads characters from the socket connection into this
buffer.*/
    struct sockaddr_in serv_addr, cli_addr;
/*
A sockaddr_in is a structure containing an internet address. This structure is
defined in netinet/in.h.
Here is the definition:
struct sockaddr_in
{
short sin_family; // must be AF_INET //
u_short sin_port;
struct in_addr sin_addr;
char sin_zero[8]; // Not used, must be zero //
};
An in_addr structure, defined in the same header file, contains only one field, a
unsigned long called s_addr.
The variable serv_addr will contain the address of the server, and cli_addr will
contain the address of the client which connects to the server.
*/
    int n; /*n is the return value for the read() and write() calls; i.e. it
contains the number of characters read or written.*/
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
/*The user needs to pass in the port number on which the server will accept
connections as an argument.
This code displays an error message if the user fails to do this.
*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
/*
The socket() system call creates a new socket. It takes three arguments.
The first is the address domain of the socket. There are two possible address domains
,
the unix domain for two processes which share a common file system, and the Internet
domain for any two hosts on the Internet.
The symbol constant AF_UNIX is used for the former, and AF_INET for the latter (there
are actually
many other options which can be used here for specialized purposes).
The second argument is the type of socket. There are two choices here, a stream
socket in which
characters are read in a continuous stream as if from a file or pipe,
and a datagram socket, in which messages are read in chunks. The two symbolic
constants are SOCK_STREAM and SOCK_DGRAM.
The third argument is the protocol. If this argument is zero (and it always should be
except
for unusual circumstances), the operating system will choose the most appropriate
protocol.
It will choose TCP for stream sockets and UDP for datagram sockets.
The socket system call returns an entry into the file descriptor table (i.e. a small
integer).
This value is used for all subsequent references to this socket. If the socket call
fails, it returns -1.
*/
    if (sockfd < 0)
        error("ERROR opening socket");
/*
if the socket call fails, displays and error message and exits.
*/
    bzero((char *) &serv_addr, sizeof(serv_addr));
/*The function bzero() sets all values in a buffer to zero.
It takes two arguments, the first is a pointer to the buffer and the second is the
size of the buffer.
Thus, this line initializes serv_addr to zeros.
*/
    portno = atoi(argv[1]);
/*
The port number on which the server will listen for connections is passed in as an
argument,
and this statement uses the atoi() function to convert this from a string of digits
to an integer.
*/
    serv_addr.sin_family = AF_INET;
/*
The variable serv_addr is a structure of type struct sockaddr_in.
This structure has four fields.
The first field is short sin_family, which contains a code for the address family.
It should always be set to the symbolic constant AF_INET.
*/
    serv_addr.sin_addr.s_addr = INADDR_ANY;
/*
The third field of sockaddr_in is a structure of type struct in_addr which contains
only a single
field unsigned long s_addr. This field contains the IP address of the host.
For server code, this will always be the IP address of the machine on which the
server is running,
F:\Presentation_courses_talks\NDSU\CSCI 459...\Tutorials\stage_1\server_Lecture.c 3
and there is a symbolic constant INADDR_ANY which gets this address.
*/
    serv_addr.sin_port = htons(portno);
/*
The second field of serv_addr is unsigned short sin_port,
which contain the port number. However, instead of simply copying the port number to
this field,
it is necessary to convert this to network byte order using the function htons()
which converts a port number in host byte order to a port number in network byte
order.
*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
/*
The bind() system call binds a socket to an address,
in this case the address of the current host and port number on which the server will
run.
It takes three arguments, the socket file descriptor, the address to which is bound,
and the size of the address to which it is bound. The second argument is a pointer to
a structure of type sockaddr,
but what is passed in is a structure of type sockaddr_in, and so this must be cast to
the correct type.
bind() returns 0 on success and -1 on falure.
*/
    while (true) {
        listen(sockfd, 5);
/*
The listen system call allows the process to listen on the socket for connections.
The first argument is the socket file descriptor, and the second is the size of the
backlog queue,
i.e., the number of connections that can be waiting while the process is handling a
particular connection.
This should be set to 5, the maximum size permitted by most systems. If the first
argument is a valid socket,
this call cannot fail, and so the code doesn't check for errors.
*/
        clilen = sizeof(cli_addr);
        newSocketFileDescriptor = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newSocketFileDescriptor < 0)
            error("ERROR on accept");

        pthread_t processThread; // this is our thread identifier
        pthread_create(&processThread, NULL, threadFunction, newSocketFileDescriptor);


/*
The accept() system call causes the process to block until a client connects to the
server.
Thus, it wakes up the process when a connection from a client has been successfully
established.
It returns a new file descriptor, and all communication on this connection should be
done using the new file descriptor.
The second argument is a reference pointer to the address of the client on the other
end of the connection,
and the third argument is the size of this structure.

        bzero(buffer, 256);
        n = read(newSocketFileDescriptor, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the message: %s\n", buffer);

Note that we would only get to this point after a client has successfully connected
to our server.
F:\Presentation_courses_talks\NDSU\CSCI 459...\Tutorials\stage_1\server_Lecture.c 4
This code initializes the buffer using the bzero() function, and then reads from
socket.
Note that the read call uses the new file descriptor, the one returned by accept(),
not the original file descriptor returned by socket(). Note also that the read() will
block
until there is something for it to read in the socket, i.e. after the client has
executed a write().
It will read either the total number of characters in the socket or 255, whichever is
less,
and return the number of characters read.

        n = write(newSocketFileDescriptor, "I got your message", 18);
        if (n < 0) error("ERROR writing to socket");

Once a connection has been established, both ends can both read and write to the
connection.
Naturally, everything written by the client will be read by the server,
and everything written by the server will be read by the client.
This code simply writes a short message to the client. The last argument of write is
the size of the message.

*/
    }

    close(newSocketFileDescriptor);
    close(sockfd);
    return 0;
}
