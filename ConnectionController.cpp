/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 19:58:28 by bworrawa         ###   ########.fr       */
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

	return connections.size();

}


bool	ConnectionController::handleRead(Connection& conn)
{
	std::cout << " *** handleRead() - Not Yet Done" << std::endl;
	size_t	bufferSize = CON_RECV_BUFFER_SIZE - 1;
	char	buffer[CON_RECV_BUFFER_SIZE];
	while(true)
	{
		int  bytesRead = read(conn.getFd(), &buffer, bufferSize);
		if(bytesRead == 0)
			return true;
		if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
			Logger::log(LC_ERROR, "CLIENT ERROR: error recv()");
            removeConnection(conn.getFd());
            return false;
        }
		// make sure it's null terminated
		buffer[bytesRead] = '\0';
		if(!conn.isHeaderComplete())
		{
			char *crlfPos = strstr(buffer, "\r\n\r\n");
			if (crlfPos)
			{

				conn.setHeaderIsComplete(true);
				
				// parse Header (to get content length)
				// rawPostBody.reserve(content length + 2), memset()

				size_t remainingHeaderLength = (crlfPos - buffer);
				conn.appendRequestBuffer( std::string(buffer).substr(0,remainingHeaderLength));
				conn.processRequestHeader( );


				
				
				conn.appendRawPostBody(crlfPos , remainingHeaderLength + 4);
				
				
			}	
		}
		else 
		{
			conn.appendRawPostBody(buffer , bytesRead);
		}

		

	}


	return (true);
}
bool	ConnectionController::handleWrite(Connection& conn)
{

	(void)conn;
	std::cout << " *** handleWrite() - Not Yet Done" << std::endl;

	return (true);
}
int	ConnectionController::addServer(int fd, ServerConfig server)
{
	if(servers.find(fd) != servers.end())
		throw std::runtime_error("ERROR, try overwrites the existing sockets");
	servers[ fd] = server;
	return servers.size();
}
ServerConfig	*ConnectionController::getServer(int fd)
{
	// if(servers.find(fd) != servers.end())
	// 	return (NULL);
	for( std::map<int, ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if(fd == it->first)
		return &(it->second);
	}
	return NULL;
}
std::map<int, ServerConfig> ConnectionController::getServers()
{
	return servers;
}

