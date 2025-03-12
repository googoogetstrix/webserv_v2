/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/12 12:02:17 by bworrawa         ###   ########.fr       */
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
	Logger::log(LC_CONN_LOG, "Closing client socket #%d, unregistererd from epoll", clientSocket);

	std::map<int,Connection>::iterator it = connections.find(clientSocket);
	epoll_ctl(epollSocket , EPOLL_CTL_DEL , clientSocket, NULL);
	close(clientSocket);	
	if(it != connections.end())
		connections.erase(clientSocket);

	return (true);
}
int		ConnectionController::openConnection(int clientSocket, ServerConfig serverConfig)
{
	int flags = fcntl(clientSocket, F_GETFL, 0);
	if (flags < 0 || fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
		close(clientSocket);
		Logger::log(LC_ERROR, "Unable to set accepted socket to non-block mode.");
		return (-1);
	}

	Logger::log(LC_RED, "RESTORE ME");

	connections[ clientSocket ] = Connection(clientSocket, serverConfig);

	epoll_event  event; 
	event.events = EPOLLIN;	
	event.data.fd = clientSocket;
	epoll_ctl(epollSocket,  EPOLL_CTL_ADD, clientSocket , &event);
	Logger::log(LC_CONN_LOG, "Accepting client connection #%d, reigistered into epoll", clientSocket);

	return connections.size();

}


bool	ConnectionController::handleRead(int clientSocket, struct epoll_event &event)
{
	Connection *conn = findConnection(clientSocket);


	size_t	bufferSize = CON_RECV_BUFFER_SIZE - 1;
	char	buffer[CON_RECV_BUFFER_SIZE];
	
	try {

		while(true)
			{
				int  bytesRead = recv(conn->getSocket(), &buffer, bufferSize, 0 );

				if(bytesRead == 0)
				{
					Logger::log(LC_CON_FAIL, "Connection disconnected from client on socket#%d" , conn->getSocket());
					closeConnection(conn->getSocket());
					return (false);
				}
					
				if (bytesRead == -1) {
					
		            if (event.events & EAGAIN || event.events & EWOULDBLOCK) {
		                return (true);
		            }
		            closeConnection(conn->getSocket());
		            return (false);
		        }
				// make sure it's null terminated
				buffer[bytesRead] = '\0';
				int pos = Util::substrPos(std::string(buffer), std::string("\r\n\r\n"));
				size_t remainingHeaderLength = bytesRead - pos;		

				if (pos != -1)
				{
					int  contentLengthFromHeader = HttpRequest::preprocessContentLength( std::string(buffer, bytesRead)); 
					size_t	actualContentLength = static_cast<size_t>(contentLengthFromHeader);
					conn->setContentLength(contentLengthFromHeader);	

					if(contentLengthFromHeader <= 0)
					{
						conn->appendRequestBuffer( std::string(buffer));
						// no Post
						conn->setHeaderIsComplete(true);
						HttpRequest httpRequest;
						httpRequest.parseRequestHeaders(conn->getServerConfig(), conn->getRequestBuffer());
						conn->processRequest(httpRequest);
						Logger::log(LC_DEBUG, " EXPECTED NORMAL close connection here");
						closeConnection(conn->getSocket());
						return (true);
					} else if( actualContentLength > conn->getRawPostBody().size())
					{
						// with Post body, splitting between header & post body here, not yet complete
						conn->appendRequestBuffer( std::string(buffer).substr(0,remainingHeaderLength));
						conn->appendRawPostBody(buffer + pos , remainingHeaderLength + 4);
					} else {
						// with Post , complete
						conn->appendRawPostBody(buffer + pos , remainingHeaderLength + 4);
					}
					
				}

				if(!conn->isHeaderComplete())
					conn->appendRequestBuffer( std::string(buffer));
				else 
					conn->appendRawPostBody(buffer , bytesRead);
			}		
			// end while true 
		
	} 
	catch (RequestException &e) {
			// This is kinda happen to partially parsed request? 
			Logger::log(LC_DEBUG, "RequestException was thrown in the main loop");
			std::cout << e.getCode() << ", " << e.getMessage() << std::endl;
			handleRequestException(e, *conn);

	}
	catch (std::exception &e)
	{
		std::cout << "EXCEPTION" << e.what() << std::endl;	
	} 
	return (true);
}

bool	ConnectionController::handleWrite(int clientSocket )
{

	Connection *conn = findConnection(clientSocket);

	if(!conn->needsToWrite())
		return (false);

	size_t sendSize = WEBS_RESP_SEND_SIZE;

	while( conn->getResponseBuffer().length() > 0 )
	{
		conn->punchIn();
 		int bytesSent = send( clientSocket , conn->getResponseBuffer().c_str() ,sendSize , MSG_DONTWAIT);
		if (bytesSent <= 0)
		{
			Logger::log(LC_MINOR_NOTE, " bytesSent = %d" , bytesSent); 
			// ## if( bytesSent == -1 && (event.events & EAGAIN  || event.events & EWOULDBLOCK))
			if( bytesSent == -1)
			{	
				Logger::log(LC_MINOR_NOTE , " Minor Error: buffer full or would block!");
				return (false);
			}
			if (bytesSent == 0)
			{
				Logger::log(LC_MINOR_NOTE , " Done sending");
				closeConnection(clientSocket);
				return (true);
			}
			
			// catch all other errors
			Logger::log(LC_CON_FAIL, "Unrecoverable socket error, abort process");
			closeConnection(conn->getSocket());
		}
		conn->truncateResponseBuffer(static_cast<size_t>(bytesSent));
		
	}
	Logger::log(LC_DEBUG, " handleWrite closing connection");
	closeConnection(clientSocket);
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

size_t	ConnectionController::purgeExpiredConnections()
{
	time_t		now = time(NULL);
	size_t 		count = 0;

	for ( std::map<int, Connection>::iterator it = connections.begin(); it != connections.end(); ++it)
	{
		if(it->second.isExpired(now) )
		{
			Logger::log(LC_NOTE, "Connection #%d timeout, closing connection and remove from epoll", it->second.getSocket());
			closeConnection(it->second.getSocket());
			count ++;
		}
	}
	return (count);
}


bool ConnectionController::handleRequestException(RequestException &e, Connection &conn)
{
	HttpResponse httpResponse;
	int statusCode = e.getCode();
	httpResponse.setStatus(e.getCode());
	// handle the 300 series which require redirection 
	if(statusCode >= 300 && statusCode <= 399)
	{
		if(!e.getMessage().empty())
			httpResponse.setHeader("Location", e.getMessage());
	}
	
	conn.ready(httpResponse);
	epoll_event  event;
	memset( &event , 0 , sizeof(event));
	handleWrite(conn.getSocket());
	return (true);
}

