/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:23:14 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/13 15:25:40 by bworrawa         ###   ########.fr       */
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

	Logger::log(LC_RED, " DEL ME , overwriting readBufferSize");
	bufferSize = 20;
	

	try {

		while(true)
			{
				int  bytesRead = recv(conn->getSocket(), &buffer, bufferSize, 0 );


				Logger:: log(LC_RED, " bytesRead = %d" , bytesRead);

				if(bytesRead == 0)
				{
					Logger::log(LC_CON_FAIL, "Connection disconnected from client on socket#%d" , conn->getSocket());
					closeConnection(conn->getSocket());
					return (false);
				}
					
				if (bytesRead == -1) {
		            if (errno == EAGAIN || errno ==  EWOULDBLOCK) 
					{
						std::cout << " EAGAIN or EWOULDBLOCK , returning false"  << std::endl;
						return (false);
					}
						

					size_t readSize = conn->getRawPostBody().size();

					std::cout << " **** rawPostBody = _" << std::string(conn->getRawPostBody().data()) << "_ " << std::endl;
					std::cout << " CHECK IF ALREADY READ ALL THE REQUIRED BYTES - me?" << std::endl;
					std::cout << " readSize = " << readSize << ", contentLength() = " << conn->getContentLength() << std::endl;					
					std::cout << "\n" << conn->getRawPostBody().data() << "\n==========XXXX" << std::endl ;

					if( readSize >= conn->getContentLength())
					{
						Logger::log(LC_DEBUG, "-1 bytes, get all the content Done reading post body!");
						conn->setRequestIsComplete(true);
						break ;

					}
					else 
						Logger::log(LC_YELLOW, "NEEDS SOME BYTES TO READ FOR POST BODY");
						
					
		            if (event.events & EAGAIN || event.events & EWOULDBLOCK) {

						if( conn->getContentLength() <= conn->getRawPostBody().size())
						{
							conn->setRequestIsComplete(true);
							break;
						}

						Logger::log(LC_RED, " EAGIAN or EWOULDBLOCK case !!!");
						return (false);
		                //return (true);
		            }
		            closeConnection(conn->getSocket());
		            return (false);
		        }
				// make sure it's null terminated
				buffer[bytesRead] = '\0';

				std::cout << " buffer for this loop = " << std::string(buffer) << std::endl;
				int pos = Util::substrPos(std::string(buffer), std::string("\r\n\r\n"));
				size_t remainingHeaderLength = bytesRead - pos;		

				if (pos != -1)
				{
					std::string method;
					conn->appendRequestBuffer(std::string(buffer, bytesRead));
					
					int  contentLengthFromHeader = HttpRequest::preprocessContentLength( conn->getRequestBuffer(), method); 
					// size_t	actualContentLength = static_cast<size_t>(contentLengthFromHeader);
					size_t	actualContentLength = contentLengthFromHeader;
					conn->setContentLength(contentLengthFromHeader);	

					if(method == "POST" && contentLengthFromHeader <= 0 )
					{
						std::cout << " NO CONTENT LENGTH CASE " << std::endl;
						conn->appendRequestBuffer( std::string(buffer));
						// no Post
						conn->setHeaderIsComplete(true);
						conn->setRequestIsComplete(true);
						break;
						// HttpRequest httpRequest;
						// httpRequest.parseRequestHeaders(conn->getServerConfig(), conn->getRequestBuffer());
						// Logger::log(LC_DEBUG, " POST stuff processRequest() call ");
						// conn->processRequest(httpRequest);
						// Logger::log(LC_DEBUG, " EXPECTED NORMAL close connection here");
						// closeConnection(conn->getSocket());
						// return (true);
					} else  if( actualContentLength > conn->getRawPostBody().size())
					{
						std::cout << " WITH CONTENT LENGTH CASE - FIRST SPLIT" << std::endl;
						// with Post body, splitting between header & post body here, not yet complete

						std::cout << " remainingLength = " << remainingHeaderLength << std::endl;
						std::cout << "buffer = " << std::string(buffer) << std::endl;

						std::cout << "\n\n=====HEAD\n" << std::string(buffer).substr(0,remainingHeaderLength) << "\n======\n\n" << std::endl;
						conn->appendRequestBuffer( std::string(buffer).substr(0,remainingHeaderLength));
						std::cout << "\n\n=====BODY\n" << std::string(buffer + pos + 4) << "\n======\n\n" << std::endl;
						conn->appendRawPostBody(buffer + pos + 4, remainingHeaderLength);
						std::cout << "\n\n=====CONBODY\n";
						

 					} else {
						// with Post , complete
						std::cout << " WITH CONTENT LENGTH CASE - FINAL TOUCH" << std::endl;
						conn->appendRawPostBody(buffer , remainingHeaderLength);
						conn->debugPostBody();
						conn->setRequestIsComplete(true);
						break; 
						
					}
					
				}

				if(!conn->isHeaderComplete())
					conn->appendRequestBuffer( std::string(buffer));
				else 
					conn->appendRawPostBody(buffer , bytesRead);
			}		
			// end while true 
		std::cout << " *** OUYT OF THE LOOP, DEALING WITH COMPLETE REQUEST " << std::endl;
		std::cout << " getRequestIsComplete "  << ( conn->getRequestIsComplete() ? "TRUE":"FALSE" ) << std::endl;
		if(conn->getRequestIsComplete())
		{
			Logger::log(LC_DEBUG, " DONE for request , now proceed to prcess it! ");
			HttpRequest httpRequest;
			httpRequest.parseRequestHeaders(conn->getServerConfig(), conn->getRequestBuffer());
			Logger::log(LC_DEBUG, " POST stuff processRequest() call ");
			conn->processRequest(httpRequest);
			Logger::log(LC_DEBUG, " EXPECTED NORMAL close connection here");
			closeConnection(conn->getSocket());
			return (true);
		}		
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

