/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tboussad <tboussad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:30:32 by tboussad          #+#    #+#             */
/*   Updated: 2025/04/11 19:56:33 by tboussad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>

class Server{
    private:
        int fd;
        int port;
        std::string password;
        std::vector<pollfd> fds;

        void setupSocket();
        void handleConnections();
        void setNonBlocking(int fd);
    
    public:
        Server(int port, const std::string& password);
        ~Server();

        void run();
        
};

#endif
