/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/13 15:04:45 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection():fd(0),isReady(false)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS);
	char buff[24];
	strftime(buff, sizeof(buff) , "[%Y-%m-%d %H:%M:%S] " , localtime(&expiresOn));
	std::cout << " A - expires on " << std::string(buff) << std::endl;
	Logger::log(LC_MINOR_NOTE, "new connection created");
}
Connection::Connection(int fd, ServerConfig config):fd(fd), serverConfig(config),isReady(false)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS);
	char buff[24];
	strftime(buff, sizeof(buff) , "[%Y-%m-%d %H:%M:%S] " , localtime(&expiresOn));
	std::cout << " B - expires on " << std::string(buff) << std::endl;
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
	expiresOn = t + (CON_SOC_TIMEOUT_SECS);
	char buff[24];
	strftime(buff, sizeof(buff) , "[%Y-%m-%d %H:%M:%S] " , localtime(&expiresOn));
	std::cout << " C - expires on " << std::string(buff) << std::endl;
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

	
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS);
	char buff[24];
	strftime(buff, sizeof(buff) , "[%Y-%m-%d %H:%M:%S] " , localtime(&expiresOn));
//	std::cout << "socket #" << fd << " expires on D - " << std::string(buff) << std::endl;
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
	std::cout << " in appendRawPostBody() ... adding:\n================== " << std::endl;
	for(size_t i = 0; i < bytesRead; ++i)
	{
		rawPostBody.push_back( *(offset + i));
		std::cout << *(offset + i);

	}
	std::cout << "\n=========\n rawPostBodyDebug = _" << std::string(rawPostBody.data()) << "_" << std::endl;
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
bool 	Connection::ready(HttpResponse &httpResponse, bool sendAsWell)
{
	isReady = true; 
	requestBuffer = "";
	
	// cannot move to response since it requires ServerConfig
	if(httpResponse.getBody().empty() && httpResponse.getStatus() >= 400 && httpResponse.getStatus() <= 599 )
		httpResponse.setBody(HttpResponse::getErrorPage(httpResponse.getStatus(), serverConfig));

	responseBuffer = httpResponse.serialize();

	if(sendAsWell)
	{
		// ignores all the closseConnection() then main loop will handle this
		while ( responseBuffer.length() > 0)
		{
			punchIn();
			size_t sendingSize = getResponseBuffer().length() < WEBS_RESP_SEND_SIZE ? getResponseBuffer().length() : WEBS_RESP_SEND_SIZE;
			int bytesSent = send( fd , getResponseBuffer().c_str() , sendingSize  , MSG_DONTWAIT);		
			if (bytesSent <= 0)
			{
				Logger::log(LC_MINOR_NOTE, " bytesSent = %d" , bytesSent); 
				if( bytesSent == -1)
				{	
					Logger::log(LC_MINOR_NOTE , " Minor Error: buffer full or would block!");
					return (false);
				}
				if (bytesSent == 0)
				{
					Logger::log(LC_MINOR_NOTE , "DONE SENDING #1, YAHOO!");					
					return (true);
				}
				
				// catch all other errors
				Logger::log(LC_ERROR, "Unrecoverable socket error, abort process");
				return (false);
			}
			truncateResponseBuffer(static_cast<size_t>(bytesSent));
		}
		return true;
	}
	
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

		Logger::log(LC_YELLOW, "Inside processRequest()");
		httpRequest.parseRequestHeaders(serverConfig , requestBuffer);

		// TODO 
		if(false) 
		{
			serverConfig.debug();
			httpRequest.debug();
			route->debug();
		}
		
		std::string path = httpRequest.getPath();		
		std::string method = httpRequest.getMethod();
		// check for malicious request, contains .. which could lead to exposing
		if (path.find("..") != std::string::npos )		
			throw RequestException(400, "Bad Request");
		
		// check for allowed methods
		std::vector<std::string> allowedMethods = route->getMethods();		
		Logger::log(LC_MINOR_NOTE, " method from header = %s" , method.c_str());
		if(!Util::strInContainer(method,  allowedMethods))
			throw RequestException(405, "Method not allowed.");

		// check if is POST , content length is required , this should be handle by handleRead already ?
		
		std::string test = httpRequest.getHeader("Content-Length");		
		if(httpRequest.getMethod() == "POST" && test.empty())
			throw RequestException(411, "Content-Length is required");
	
		// also check for body too large
		// route->debug();
		size_t maxSize = route->getClientMaxBodySize();
		if(maxSize == 0)
			maxSize = WEBS_DEF_MAX_BOD_SIZE;
		maxSize *= WEBS_MB;

		if(!test.empty() && Util::toSizeT(test) > maxSize)
			throw RequestException(415, "Request too large");	

		// check for redirection (directive return)	
		if(route->getReturnStatus() != 0)
		{
			int statusCode = route->getReturnStatus();
			// is one of the redirections
			if(statusCode >= 300 and statusCode <= 399)
			{
				if(!route->getReturnValue().empty())
					throw RequestException(statusCode , route->getReturnValue());
			}
			throw RequestException(statusCode , "");

		}



		std::string  localPath = "";
		bool		 allowDirectoryBrowsing = false;
		if(!serverConfig.resolveRoute(httpRequest, *route, localPath , allowDirectoryBrowsing))
			throw RequestException(403, "Forbidden");

		Logger::log(LC_NOTE, "Request seems OK so far");	
		std::cout << " ProcessRequest() localPath is " << localPath << std::endl;

		
		std::string requestPathContainFile = Util::extractFileName( localPath, true);
		if(requestPathContainFile.empty())
		{
			if(!allowDirectoryBrowsing)
				throw RequestException(403, "Forbidden");
			httpResponse.generateDirectoryListing(httpRequest, localPath);
		} 
		else 
		{
			std::string cmd = route->getCGI(Util::getFileExtension(requestPathContainFile));
			
			if(!cmd.empty())
			{
				// is CGI
				Logger::log(LC_RED, "%s is CGI , with command %s ", localPath.c_str(), cmd.c_str());
				httpResponse.processPythonCGI( cmd , localPath, httpRequest, serverConfig , *route , rawPostBody);
			}
			else
			{
				Logger::log(LC_YELLOW, "%s is static file ", localPath.c_str());
				httpResponse.getStaticFile(localPath);

			}

		}	
		Logger::log(LC_DEBUG, "Response is ready!");
		ready(httpResponse, true);
		return (true);
}

void 	Connection::setContentLength(int i)
{
		contentLength = i;
}
size_t		Connection::getContentLength()
{
		return contentLength; 
}


std::vector<char>	&Connection::getRawPostBody()
{
	return rawPostBody;
}
bool				Connection::isExpired(time_t comp) const
{
	return expiresOn < comp;
}

void Connection::debugPostBody()
{
	size_t  daSize = 0 ;
	for(std::vector<char>::iterator it = rawPostBody.begin(); it != rawPostBody.end(); ++it)
	{
		std::cout << *it;
		daSize ++; 
	}
	std::cout << "\n size=" << daSize << std::endl;
}
bool Connection::getHeaderIsComplete(void) const
{
	return headerIsCompleted;
}


bool Connection::getRequestIsComplete(void) const
{
	return requestIsCompleted;
}


void Connection::setRequestIsComplete(bool newValue)
{
	requestIsCompleted = newValue;
}
