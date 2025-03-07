/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/07 18:04:29 by bworrawa         ###   ########.fr       */
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


bool	ConnectionController::handleRead(Connection& conn, struct epoll_event &event, HttpRequest &httpRequest, HttpResponse &httpResponse)
{

	(void) event;
	std::cout << " *** handleRead() - Not Yet Done" << std::endl;
	size_t	bufferSize = CON_RECV_BUFFER_SIZE - 1;
	char	buffer[CON_RECV_BUFFER_SIZE];


	// ### DELME 
	bufferSize = 20;

	try {

		while(true)
			{
				int  bytesRead = recv(conn.getFd(), &buffer, bufferSize, 0 );
				std::cout << " bytesRead = " << bytesRead << std::endl;
				if(bytesRead == 0)
					break; 
				if (bytesRead == -1) {
					// Done reading!
		            if (errno == EAGAIN || errno == EWOULDBLOCK) {
		                break;
		            }
					Logger::log(LC_ERROR, "CLIENT ERROR: error recv()");
		            closeConnection(conn.getFd());
		            return false;
		        }
				// make sure it's null terminated
				buffer[bytesRead] = '\0';
				int pos = Util::substrPos(std::string(buffer), std::string("\r\n\r\n"));
				size_t remainingHeaderLength = bytesRead - pos;		

				// if found \r\n\r\n in current loop, split into string (requestBuffer) and vector<char> rawPostBody
				if (pos != -1)
				{
					conn.setHeaderIsComplete(true);
					conn.appendRequestBuffer( std::string(buffer).substr(0,remainingHeaderLength));
					conn.appendRawPostBody(buffer + pos , remainingHeaderLength + 4);

					ServerConfig 	*server = getServer(conn.getFd());
					httpRequest.parseRequestHeaders(httpResponse, *server , conn.getRequestBuffer());
					httpRequest.debug();
				}

				if(!conn.isHeaderComplete())
					conn.appendRequestBuffer( std::string(buffer));
				else 
					conn.appendRawPostBody(buffer , bytesRead);
			}

			
			// end while true 
	} catch (std::exception &e)
	{
		std::cout << "EXCEPTION" << e.what() << std::endl;	

	} 
	

	return (true);
}

bool	ConnectionController::handleWrite(Connection& conn, struct epoll_event& event ,HttpResponse &httpResponse)
{
	conn.ready(httpResponse);
	if(!conn.needsToWrite())
		return (false);

	size_t sendSize = conn.getResponseBuffer().length();	

	while( conn.getResponseBuffer().length() > 0 )
	{
		conn.punchIn();
 		int bytesSent = send( event.data.fd , conn.getResponseBuffer().c_str() ,sendSize , MSG_DONTWAIT);
		if (bytesSent <= 0)
		{
			Logger::log(LC_RED, " bytesSent = %d" , bytesSent); 
			if( bytesSent == -1 && (event.events & EAGAIN  || event.events & EWOULDBLOCK))
			{	
				Logger::log(LC_NOTE , " Minor Error: buffer full or would block!");
				return (false);
			}
			if (bytesSent == 0)
			{
				Logger::log(LC_NOTE , "DONE SENDING #1, YAHOO!");
				ConnectionController::closeConnection(event.data.fd);
				return (true);
			}
			
			// catch all other errors
			Logger::log(LC_ERROR, "Unrecoverable socket error, abort process");
			ConnectionController::closeConnection(conn.getFd());
		}
		// size_t compareSize = static_cast<size_t>(bytesSent);
		// compareSize = compareSize < conn.getResponseBuffer().length() ? compareSize : conn.getResponseBuffer().length();
		// responseBuffer =  responseBuffer.substr(compareSize); 
		conn.truncateResponseBuffer(static_cast<size_t>(bytesSent));
		
	}
	ConnectionController::closeConnection(event.data.fd);
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
