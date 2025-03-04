/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 18:54:14 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection():fd(0),lastActive(time(0))
{

}
Connection::Connection(int fd, ServerConfig config):fd(fd),lastActive(time(0)), config(config)
{
	setNonBlock();
}
Connection::~Connection()
{

}
Connection::Connection(Connection const &other)
{
	fd = other.fd;
	lastActive = other.lastActive;
	config = other.config;

}
Connection &Connection::operator=(Connection const other)
{
	fd = other.fd;
	lastActive = other.lastActive;
	config = other.config;
	return (*this);
}

void Connection::setNonBlock()
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags >= 0) {
		if( fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
			throw std::runtime_error("Unable to set socket to non-blocking mode");
	}
}


time_t	Connection::getLastActive() const 
{
	return (lastActive);
}

int 	Connection::getFd() const
{
	return (fd);
}

bool 	Connection::setLastActive(time_t t)
{
	lastActive = t;
	return true; 
}

bool 	Connection::setFd(int newFd)
{
	fd = newFd;
	return true;
}

void	Connection::punchIn(void)
{
	lastActive = time(NULL);
}
