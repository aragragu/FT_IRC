/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aragragu <aragragu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:56:10 by tboussad          #+#    #+#             */
/*   Updated: 2025/04/13 14:31:44 by aragragu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <poll.h>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Server::Server(int port, const std::string& password) : fd(-1), port(port), password(password){
    setupSocket();
}

Server::~Server(){
    if (fd >= 0)
        close(fd);
}


void Server::setNonBlocking(int fd){
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Server::setupSocket(){
    int opt = 1;

    /* Socket:
        -1- domain
        -2- type(STREAM / DGRAM)
        -3- protocol
    */
    this->fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET for IPv4 ou SOCK_STREAM b7al TCP
    // ou 0 to let the system choose the appropriate protocol to send data

    if(this->fd < 0){
        std::cout << "error: creating socket" << std::endl;
        exit(EXIT_FAILURE);
        // throw std::runtime_error("Failed to create socket");
    }

    /* setsockopt:
        -1- socket fd
        -2- level (SOL_SOCKET for general socket options)
        -3- option name (SO_REUSEADDR to allow reuse of local addresses)
        -4- option value (pointer to the value)
        -5- option value size (size of the value)
    */
    setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // set socket option to reuse the address

    setNonBlocking(this->fd); // set the socket to non-blocking mode

    // sockaddr_in b7al chi address postal dial network cat holdi l IP @ ou l port
    // number li program dialna ghadi i usi bach it connecta wla i listeni l other programms
    struct sockaddr_in serv_address;

    /*
        struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
        };

        struct in_addr {
            unsigned long s_addr;  // load with inet_aton()
};*/
    std::memset(&serv_address, 0, sizeof(serv_address));
    // memset: to set all bytes of the serv_address structure to 0
    /* sockaddr_in
        -1- family (AF_INET for IPv4)
        -2- port (htons to convert to network byte order)
        -3- address (INADDR_ANY to bind it all)
    */
    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(port);
    serv_address.sin_addr.s_addr = INADDR_ANY;

    // bind() assigns a local address and port to the socket
    if (bind(this->fd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0){
        std::cout << "error: failed to bind" << std::endl;
        exit(EXIT_FAILURE);
        // throw std::runtime_error("error: failed to bind");
    }

    // listen b7ala kat marki socket 3la anaha t usa ou tbda tsm3 l connections
    // listen() puts a socket into a passive listening mode.
    /* listen:
        -1- socket fd
        -2- backlog (number of connections that can be queued)
    */
    if (listen(this->fd, 10) < 0){
        std::cout << "error: failed to listen" << std::endl;
        exit(EXIT_FAILURE);
        // throw std::runtime_error("error: failed to listen");
    }

    // pollfd ghir wahd structure bach poll() t9der t usi bach tmonitori sockets
    // dial server dialna for incoming events
    /* pollfd:
        -1- fd (hnaya l fd lbghina poll i7di 3la l events)
        -2- events (events to monitor)
        -3- revents (returned events)
    */
    struct pollfd pfd;
    pfd.fd = this->fd;
    pfd.events = POLLIN; // POLLIN is a constant that tells poll() you're interested
    // in events that indicate data is ready to be read
    pfd.revents = 0;
    this->fds.push_back(pfd);

    std::cout << "[SBITAR] Server listening on port " << port << std::endl;
}

void Server::run(){
    while(true){
        int ready = poll(&fds[0], fds.size(), -1); // wait indefinitely for events
        if(ready < 0){
            // perror("error: poll failed");
            exit(EXIT_FAILURE);
            // throw std::runtime_error("error: poll failed");
        }
        for(size_t i=0;i < fds.size(); ++i){
            if(fds[i].revents & POLLIN){
                if(fds[i].fd == fd)
                    handleConnections();
                else{
                    char buf[1024];
                    int bytes = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);
                    if(bytes <= 0){
                        close(fds[i].fd);
                        std::cout << "[SBITAR] client disconnected" << std::endl;
                        fds.erase(fds.begin() + i);
                        --i; // adjust index after erasing
                    }
                    else{
                        buf[bytes] = '\0'; // null-terminate the received data
                        std::cout << "[SBITAR] Received: " << buf << std::endl;
                        // Here you can handle the received data, e.g., send a response
                        send(fds[i].fd, buf, bytes, 0); // echo back
                    }
                }
            }
        }
    }
}

void Server::handleConnections(){
    struct sockaddr_in client_address;
    socklen_t client_lenght = sizeof(client_address);
    // accept() creates a new socket for the incoming connection
    // accept() blocks until a connection is made
    /* accept:
        -1- socket fd
        -2- client address (pointer to the client address structure)
        -3- client address length (pointer to the length of the client address structure)
    */
    int client_fd = accept(fd, (struct sockaddr *)&client_address, &client_lenght);
    if(client_fd < 0){
        std::cout << "error: failed to accept connection" << std::endl;
        return ;
        // throw std::runtime_error("error: failed to accept connection");
    }
    setNonBlocking(client_fd);
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    fds.push_back(pfd);

    std::cout << "[SBITAR] New client connected (fd = " << client_fd << ")\n";
}
