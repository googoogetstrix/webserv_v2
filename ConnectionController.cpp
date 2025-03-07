/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/06 19:10:13 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionController.hpp"
#include "Logger.hpp"

int ConnectionController::epollSocket = 0; 

ConnectionController::ConnectionController(ConnectionController const &other)
{
	(void) other;
}
ConnectionController &ConnectionController::operator=(ConnectionController const &other)
{
	(void) other;
	return (*this);
}


ConnectionController::ConnectionController()
{
	
}

// ConnectionController::ConnectionController(int timeout):timeoutInSecs(timeout)
// {
	
// }

ConnectionController::~ConnectionController()
{
	
}


Connection *ConnectionController::findConnection(int fd)
{
	std::map<int,Connection>::iterator it = connections.find(fd);
	if(it != connections.end())
		return &(it->second);	
	return (NULL);
}
bool	ConnectionController::closeConnection(int clientSocket)
{		
	Logger::log(LC_NOTE, "Closing client socket #%d, unregistererd from epoll", clientSocket);
	epoll_ctl(epollSocket , EPOLL_CTL_DEL , clientSocket, NULL);
	close(clientSocket);
	return (true);
}
int		ConnectionController::openConnection(int clientSocket, ServerConfig config)
{
	int flags = fcntl(clientSocket, F_GETFL, 0);
	if (flags < 0 || fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
		close(clientSocket);
		Logger::log(LC_ERROR, "Unable to set accepted socket to non-block mode.");
		return (-1);
	}

	// connections.at(fd) = Connection(fd, config);
	connections[ clientSocket ] = Connection(clientSocket, config);


	for( std::map<int,Connection>::iterator it = connections.begin(); it != connections.end(); ++it)
	{
		std::cout << " KEY = " << it->first << std::endl;
	}

	return connections.size();

}


bool	ConnectionController::handleRead(Connection& conn, struct epoll_event &event)
{

	(void) event;
	std::cout << " *** handleRead() - Not Yet Done" << std::endl;
	size_t	bufferSize = CON_RECV_BUFFER_SIZE - 1;
	char	buffer[CON_RECV_BUFFER_SIZE];


	try {

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
		            closeConnection(conn.getFd());
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
						
			
						conn.appendRawPostBody(crlfPos , remainingHeaderLength + 4);
						
						
					}	
				}
				else 
				{
					conn.appendRawPostBody(buffer , bytesRead);
				}

				

			}
	} catch (std::exception &e)
	{
		std::cout << "EXCEPTION" << e.what() << std::endl;	

	} 
	


	return (true);
}
bool	ConnectionController::handleWrite(Connection& conn, struct epoll_event& event)
{

	(void)conn;
	(void) event;

	
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


void		ConnectionController::setEpollSocket(int epollFd)
{
	epollSocket = epollFd;
}

int		ConnectionController::getEpollSocket()
{

	return (epollSocket);
}
