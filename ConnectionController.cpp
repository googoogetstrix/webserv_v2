/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 09:46:29 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionController.hpp"
#include "Logger.hpp"

ConnectionController::ConnectionController(ConnectionController const &other)
{
	(void) other;
}
ConnectionController &ConnectionController::operator=(ConnectionController const &other)
{
	(void) other;
	return (*this);
}


ConnectionController::ConnectionController():timeoutInSecs(CC_DEF_TIMEOUT_IN_SEC)
{
	
}

ConnectionController::ConnectionController(int timeout):timeoutInSecs(timeout)
{
	
}

ConnectionController::~ConnectionController()
{
	
}


Connection *ConnectionController::findConnection(int fd)
{
	std::cout << "*** FINDING " << fd << std::endl;
	{
		for( std::map<int,Connection>::iterator it = connections.begin(); it != connections.end(); ++it)
		{
			std::cout << " KEY " << it->first << std::endl;
		}
	}


	std::map<int,Connection>::iterator it = connections.find(fd);
	if(it != connections.end())
		return &(it->second);
	return (NULL);
}
bool	ConnectionController::removeConnection(int fd)
{	
	(void) fd;
	(void) timeoutInSecs;
	
	std::cout << " NOT YET IMPLEMENTED!!!!!!!!!!!!!!!!" << std::endl;
	return (true);
}
int		ConnectionController::addConnection(int fd, ServerConfig config)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		close(fd);
		Logger::log(LC_ERROR, "Unable to set accepted socket to non-block mode.");
		return (-1);
	}

	// connections.at(fd) = Connection(fd, config);
	connections[ fd ] = Connection(fd, config);


	for( std::map<int,Connection>::iterator it = connections.begin(); it != connections.end(); ++it)
	{
		std::cout << " KEY = " << it->first << std::endl;
	}
//	 throw std::runtime_error("GODDAMNNNNNN");
	return connections.size();

}
