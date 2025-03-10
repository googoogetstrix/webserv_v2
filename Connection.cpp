/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/10 15:15:45 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection():fd(0),isReady(false)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS * 100000);
	Logger::log(LC_MINOR_NOTE, "new connection created");
}
Connection::Connection(int fd, ServerConfig config):fd(fd), serverConfig(config),isReady(false)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS * 100000);
	setNonBlock();
	Logger::log(LC_MINOR_NOTE, "new connection with fd#%d created", fd);
}
Connection::~Connection()
{
	Logger::log(LC_MINOR_NOTE, "connection #%d destroyed", fd);
}
Connection::Connection(Connection const &other)
{
	fd = other.fd;
	expiresOn = other.expiresOn;
	serverConfig = other.serverConfig;
	isReady = other.isReady; 
	bodyLength = other.bodyLength; 
	requestBuffer = other.requestBuffer;
	responseBuffer = other.responseBuffer;
	rawPostBody = other.rawPostBody;
	contentLength = other.contentLength;
	epollSocket = other.epollSocket; 

}
Connection &Connection::operator=(Connection const other)
{
	fd = other.fd;
	expiresOn = other.expiresOn;
	serverConfig = other.serverConfig;
	isReady = other.isReady; 
	bodyLength = other.bodyLength; 
	requestBuffer = other.requestBuffer;
	responseBuffer = other.responseBuffer;
	rawPostBody = other.rawPostBody;
	contentLength = other.contentLength;
	epollSocket = other.epollSocket; 
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


time_t	Connection::getExpiresOn() const 
{
	return (expiresOn);
}

int 	Connection::getSocket() const
{
	return (fd);
}

bool 	Connection::setExpiresOn(time_t t)
{
	expiresOn = t + (CON_SOC_TIMEOUT_SECS * 100000);
	return true; 
}

bool 	Connection::setFd(int newFd)
{
	fd = newFd;
	return true;
}

void	Connection::punchIn(void)
{
	// Logger::log(LC_NOTE, "Punch in!");
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS * 1000000);
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


	try 
	{
	while(  std::getline(headersStream , line))
		{
			lineNo++;
			if (lineNo == 1)
			{	
				std::istringstream  lineStream; 
				std::string  method , target , version;


				if(!(lineStream >> method >> target >> version))
				{

					throw std::runtime_error("dddd");
					
					// httpResponse.setStatus(400);
					// httpResponse.setBody(httpResponse.getDefaultErrorPage(400));
					// return false; 
				}
				
			}

			std::cout << "HEADER!!!" << line << std::endl;
		}

		
	}
	catch (std::exception &e)
	{
			std::cerr << " ERROR " << e.what() << std::endl;
	}

	return (true);
	
}
bool 	Connection::ready(HttpResponse &httpResponse)
{
	isReady = true; 
	requestBuffer = "";
	
	if(httpResponse.getBody().empty() && httpResponse.getStatus() >= 400 && httpResponse.getStatus() <= 599 )
	{
		std::cout << " STILL NEEDS TO SET EMPTY BODY " << std::endl;
		httpResponse.setBody(HttpResponse::getErrorPage(httpResponse.getStatus(), serverConfig));
	}
	responseBuffer = httpResponse.serialize();
	return true;
	

}

bool	Connection::getIsReady() const 
{
	return (isReady); 
}

void 	Connection::setIsReady(bool newValue)
{
	isReady  = newValue;
}

bool	Connection::needsToWrite()
{
	return (!responseBuffer.empty());
}


// bool 	Connection::handleWrite( int epoll_fd, struct epoll_event &event)
// {
// 	Logger::log(LC_RED, "Moved to COnnectionController->handleRead");
// 	return false;

// 	(void) epoll_fd;
// 	if(!needsToWrite())
// 		return (false);

// 	size_t sendSize = responseBuffer.length();	
// 	while( responseBuffer.length() > 0 )
// 	{
// 		punchIn();
// 		// if(sendSize < responseBuffer.length())
// 		// 	sendSize = responseBuffer.length();
//  		int bytesSent = send( event.data.fd , responseBuffer.c_str() ,sendSize , MSG_DONTWAIT);
// 		if (bytesSent <= 0)
// 		{
// 			Logger::log(LC_RED, " bytesSent = %d" , bytesSent); 
// 			if( bytesSent == -1 && (event.events & EAGAIN  || event.events & EWOULDBLOCK))
// 			{	
// 				Logger::log(LC_NOTE , " Minor Error: buffer full or would block!");
// 				return (false);
// 			}
// 			if (bytesSent == 0)
// 			{
// 				Logger::log(LC_NOTE , "DONE SENDING #1, YAHOO!");
// 				ConnectionController::closeConnection(event.data.fd);
// 				return (true);
// 			}
			
// 			// catch all other errors
// 			Logger::log(LC_ERROR, "Unrecoverable socket error, abort process");
// 			ConnectionController::closeConnection(fd);
// 		}
// 		size_t compareSize = static_cast<size_t>(bytesSent);
// 		compareSize = compareSize < responseBuffer.length() ? compareSize : responseBuffer.length();
// 		responseBuffer =  responseBuffer.substr(compareSize); 
		
// 	}
// 	ConnectionController::closeConnection(event.data.fd);
// 	return (true);
	
// }


size_t	Connection::truncateResponseBuffer(size_t bytesSent)
{

		bytesSent = bytesSent < responseBuffer.length() ? bytesSent : responseBuffer.length();
		responseBuffer =  responseBuffer.substr(bytesSent); 	
		return responseBuffer.size();
}

std::string 		Connection::getResponseBuffer() const
{
	return (responseBuffer);
}

std::string 		Connection::getRequestBuffer() const
{
	return (requestBuffer);
}




ServerConfig		&Connection::getServerConfig()
{
	// if(serverConfig.getPort() != 0)
	// 	return &serverConfig;
	// return  NULL ; 
	return serverConfig;
}


bool	Connection::processRequest(HttpRequest &httpRequest)
{
		RouteConfig *route = serverConfig.findRoute(httpRequest.getPath());

		HttpResponse httpResponse;
	

		// try check all the error could possibly happen

		// 400 Bad request
		// 401 Unauthorized
		// 403 Forbidden - since there's no login , al should be handled by 401?
		// 404 Not Found
		// 405 Method Not Allowed
		// 411 Length Required
		// 413 Content Too Large
		// 414 URI Too Long
		// 500 Internal Server Error

		std::cout << "request_buffer: " << requestBuffer << std::endl;

		httpRequest.parseRequestHeaders(serverConfig , requestBuffer);

		// TODO 
		if(false) 
		{
			serverConfig.debug();
		}
		httpRequest.debug();
		route->debug();
		


		std::string path = httpRequest.getPath();		
		std::string method = httpRequest.getMethod();
		if (path.find("..") != std::string::npos )		
			throw RequestException(400, "Bad Request");
		
		std::vector<std::string> allowedMethods = route->getMethods();
		if(Util::strInContainer(method,  allowedMethods))
			throw RequestException(405, "Method not allowed.");
//			return httpResponse.setStatus(405) && false; 


		std::string test = httpRequest.getHeader("Content-Length");
		if(httpRequest.getMethod() == "POST" && test.empty())
			return httpResponse.setStatus(411) && false; 
		size_t maxSize = route->getClientMaxBodySize();
		if(maxSize == 0)
			maxSize = WEBS_DEF_MAX_BOD_SIZE;
		maxSize *= WEBS_MB;
		if(Util::toSizeT(test) > maxSize * WEBS_MB)
			return httpResponse.setStatus(415) && false; 



		Logger::log(LC_RED, " NEEDS CHECK IF IS CGI ?");
		std::string m = httpRequest.getMethod() ;
		Logger::log(LC_RED, " requestMethod = %s " , m.c_str());




		Logger::log(LC_GREEN, "Request seems OK so far");

		std::string  localPath = "";
		bool		 allowDirectoryBrowsing = false;
		if(!serverConfig.resolveRoute(httpRequest, *route, localPath , allowDirectoryBrowsing))
			throw RequestException(403, "Forbidden");

		std::cout << " ProcessRequest() localPath is " << localPath << std::endl;

	
	
		httpResponse.getStaticFile(httpRequest, serverConfig , route);
		throw RequestException(httpResponse.getStatus(), "OK");
		
		

				
	



		Logger::log(LC_RED, " DEL ME :: Connection::processRequest() ==> WORKING_HERE_EXCEPTION, no needs to go any further");
		throw RequestException(599, "599");
}

void 	Connection::setContentLength(int i)
{
		contentLength = i;
}
int		Connection::getContentLength()
{
		return contentLength; 
}


std::vector<char>	&Connection::getRawPostBody()
{
	return rawPostBody;
}
