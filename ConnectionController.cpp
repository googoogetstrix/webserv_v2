/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 19:28:05 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionController.hpp"

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
	std::map<int,Connection>::iterator it = connections.find(fd);
	{
		if (it->first == fd)
			return &(it->second);
	}
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
	}

	connections.at(fd) = Connection(fd, config);
	return connections.size();

}
