/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboussad <tboussad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:05 by tboussad          #+#    #+#             */
/*   Updated: 2025/04/11 19:42:42 by tboussad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/*  * Creates a TCP server socket

    * Binds to a port (default: 6667)
    
    * Listens for incoming connections
    
    * Accepts new clients
    
    * Uses poll to handle multiple clients
    
    * Echoes messages back to clients (temporarily — until we implement command parsing)
*/
#include "Server.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

int main(int argc, char **argv){
    if (argc != 3){
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    if (port <= 0 || port > 65535){
        std::cerr << "error: invalid port number\n";
        return 1;
    }

    try {
        Server server(port, password);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}