#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /*The file netdb.h defines the structure hostent, which will be
used below.*/
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
/*
The variable server is a pointer to a structure of type hostent.
This structure is defined in the header file netdb.h as follows:
struct hostent
{
char *h_name; // official name of host /
char **h_aliases; // alias list //
int h_addrtype; // host address type //
int h_length; // length of address //
char **h_addr_list; // list of addresses from name server //
#define h_addr h_addr_list[0] // address, for backward compatiblity //
};
It defines a host computer on the Internet. The members of this structure are:
h_name Official name of the host.
h_aliases A zero terminated array of alternate
names for the host.
h_addrtype The type of address being returned;
currently always AF_INET.
h_length The length, in bytes, of the address.
h_addr_list A pointer to a list of network addresses
for the named host. Host addresses are
returned in network byte order.
Note that h_addr is an alias for the first address in the array of network addresses.
*/
    char buffer[256];
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
/*
The variable argv[1] contains the name of a host on the Internet, e.g. cs.rpi.edu. The
function:
struct hostent *gethostbyname(char *name)
Takes such a name as an argument and returns a pointer to a hostent containing
information about that host.
The field char *h_addr contains the IP address.
If this structure is NULL, the system could not locate a host with this name.
*/
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
/*
This code sets the fields in serv_addr. Much of it is the same as in the server.
However, because the field server->h_addr is a character string, we use the function:
void bcopy(char *s1, char *s2, int length)
which copies length bytes from s1 to s2.
*/
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
/*
The connect function is called by the client to establish a connection to the server.
It takes three arguments, the socket file descriptor,
the address of the host to which it wants to connect (including the port number),
and the size of this address. This function returns 0 on success and -1 if it fails.
Notice that the client needs to know the port number of the server,
but it does not need to know its own port number. This is typically assigned by the
system when connect is called.
*/

char exitBuffer[2];
strcpy(exitBuffer, "EXIT");

    while (1)
    {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
		
		//printf("%s",exitBuffer);
		
		//Int for comparison of EXIT, and Exit, and exit
		int value = strcmp(buffer, exitBuffer);
		int value1 = strcmp(buffer, exitBuffer);
		int value2 = strcmp(buffer, exitBuffer);
		
		printf("The did you send EXIT?: %i\n", value);
		printf("The did you send Exit?: %i\n", value1);
		printf("The did you send exit?: %i\n", value2);		
		
        if (strcmp(buffer, exitBuffer) == 10)
        {
        	printf("Input has been caputred: %s\n", buffer);
        	
            write(sockfd,buffer,strlen(buffer));
            break;
        }
        
        printf("Input is about to be sent: %s\n", buffer);

        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
        
        printf("Input is has been sent: %s\n", buffer);
		bzero(buffer,256);
                
        
        n = read(sockfd,buffer,255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n",buffer);
    }

/*
The remaining code should be fairly clear.
It prompts the user to enter a message, uses fgets to read the message from stdin,
writes the message to the socket, reads the reply from the socket, and displays this
reply on the screen.
*/

printf("I AM OUTSIDE THE LOOP\n");
    close(sockfd);
    return 0;
}
