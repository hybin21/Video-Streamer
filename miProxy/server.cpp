// resolves the web server's DNS name
// opens a conenction with the resulting IP address
// accepts connections from web browers
// modifies video segment requests as describe ebloe before forwarding them to the web server
    // any data returned by the server(the vid segments themselves) should be forwarded, unmodified, to the browser

// miProxy should listen for browese connections on the IP nd prot specified on the command line
// connect to a web server either directly specified on the comman line(by its IP address)
    // if not issue a DNS query to find out the IP address of the server to contant(FOR PART B)

// assign ephemeral is referring to the fact that, for the client side of the connection,
    // you dont need to bind to any specific prot
    // kernel will pck the proxy's TCP port when it connets to the web server
        // => nth more than the proxy calling connect() is happening here

// our miProxy should accept multiple concurrent connections from clients(web browsers) using select()
// be able to handle the required HTTP 1.1 requests for this assignment (HTTP GET)

#include <arpa/inet.h> //close
#include <array>       // std::array
#include <cstdio>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO, FD_SETSIZE macros
#include <sys/types.h>
#include <unistd.h> //close

#define PORT  6000
#define MAXCLIENTS 30

/*
    Implement a simplified CDN. 
    - first, entire system will run on one host and rely on Mininet to run serveral processes
    with arbitraty IP addresses on one machine 
        - mininet will allow you to assign arbitrary link characteristics(bandwidth and latency) 
        to each pair of "end hosts" (processes)
    - Draw DNS Server and Proxy 
    1. PART A: Bitrate Adaptation in HTTP Proxy 
    2. PART B: DNS Load Balancing 
*/

int getSocket(struct sockaddr_in *addy){
    int yes(1);
    int mySocket(0);

    mySocket = socket(AF_INET, SOCK_STREAM,0);
    if(mySocket <= 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int success =  setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (success < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
    }

    // type of socket created
    addy->sin_family = AF_INET;
    addy->sin_addr.s_addr = INADDR_ANY;
    addy->sin_port = htons(PORT);

    success = ::bind(mySocket, (struct sockaddr *) addy, sizeof(*addy));
    if(success < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    print("---------Listening on Port %d---------\n",PORT);
    // try to specify max of 3 pending connections for the master socket
    if(listen(mySocket,3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return mySocket;
}

int main(int argc, char * argv[]){
    int mySocket, addrlen, activity, valread;
    std::array<int,MAXCLIENTS> clientSockets;

    struct sockaddr_in address;
    mySocket = getSocket(&address);

    char buffer[1025]
    const char * message = "DJJ CDN v1.0\r\n";  // idk what to put as a message, should be changed

    clientSockets.fill(0);
    // accept the incoming connection 
    addrlen = sizeof(address);
    puts("waiting for connections ...");
    // set of socket descritpors
    fd_set readfds;
    while(true){
        // clear the socket set
        FD_ZERO(&readfds);
        // add master socket to set
        FD_SET(mySocket, &readfds);
        for(const auto & socket: clientSockets){
            if(socket)
                FD_SET(socket,&readfds);
        }
        // wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely 
        activity = select(FD_SETSIZE, &readfds, NULL,NULL,NULL);
        if((activity < 0) && (errno != EINTR) )
            perror("select error");

        // if sth happened ont he master socket, then its an icoming connection, call accept()
        if(FD_ISSET(mySocket, &readfds)){
            int newSocket = accept(mySocket, (struct sockaddr *) & address, (socklen_t * ) & addrlen);
            if(newSocket < 0){
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            printf("\n---New host connection---\n");
            printf("socket fd is %d , ip is : %s , port : %d \n", new_socket,inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // IO operation on a client socket
            for(auto & socket: clientSockets){
                if(socket && FD_ISSET(socket, &readfds)){
                    // check if it was for closing, and also read the incoming message
                    getpeername(socket, (struct sockaddr *)&address,
                    (socklen_t *)&addrlen);
                    valread = read(client_sock, buffer, 1024);
                    if(!valread){
                        // somebody disconnected, get their details and print
                        printf("\n---Host disconnected---\n");
                        printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                        // close the socket and mark as 0 in list for reuse
                        close(socket);
                        socket = 0;
                    }
                    else{
                        // send the same message back to the client
                    }
                }
            }
        }

    }
}

