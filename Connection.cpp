/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 18:32:42 by bworrawa         ###   ########.fr       */
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


void	Connection::setHeaderIsComplete(bool newValue){
	headerIsCompleted = newValue;
	
}
bool	Connection::isHeaderComplete()
{
	return (headerIsCompleted);
}

bool	Connection::appendRawPostBody(char *offset, size_t bytesRead)
{
	for(size_t i = 0; i < bytesRead; ++i)
	{
		rawPostBody.push_back( *(offset + i));
	}
	return (true);
}
bool	Connection::appendRequestBuffer(std::string str)
{
	requestBuffer += str;
	return (true);
}


bool	Connection::processRequestHeader()
{
	std::istringstream  headersStream(requestBuffer);
	
	std::string 		line;

	size_t				lineNo = 0;

	while(  std::getline(headersStream , line))
	{
		lineNo++;
		if (lineNo == 1)
		{	
			std::istringstream  lineStream; 
			std::string  method , target , version;


			if(!(lineStream >> method >> target >> version))
			{
				httpResponse.setStatus(400);
			}
			
		}

		std::cout << "HEADER!!!" << line << std::endl;
	}

	return (true);
}
