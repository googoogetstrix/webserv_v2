/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/06 11:46:08 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection():fd(0)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS * 100000);
}
Connection::Connection(int fd, ServerConfig config):fd(fd), config(config)
{
	expiresOn = time(NULL) + (CON_SOC_TIMEOUT_SECS * 100000);
	setNonBlock();
}
Connection::~Connection()
{

}
Connection::Connection(Connection const &other)
{
	fd = other.fd;
	expiresOn = other.expiresOn;
	config = other.config;

}
Connection &Connection::operator=(Connection const other)
{
	fd = other.fd;
	expiresOn = other.expiresOn;
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


time_t	Connection::getExpiresOn() const 
{
	return (expiresOn);
}

int 	Connection::getFd() const
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
	Logger::log(LC_NOTE, "Punch in!");
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
				httpResponse.setBody(httpResponse.getDefaultErrorPage(400));
				return false; 
			}
			
		}

		std::cout << "HEADER!!!" << line << std::endl;
	}

	return (true);
}
bool 	Connection::ready(struct epoll_event &event, bool startResponseNow=true)
{
	responseBuffer = httpResponse.serialize();
	std::cout << "\n\n\nRESPONSE BUFFER\n" << responseBuffer << "\n\n" << std::endl;
 	if(startResponseNow)
		handleWrite(event);
	return (true);

}

bool	Connection::needsToWrite()
{
	return (responseBuffer.empty());
}


bool 	Connection::handleWrite(struct epoll_event &event)
{
	if(!needsToWrite())
		return (false);

	
	size_t sendSize = responseBuffer.length();
	sendSize = 20;
	while( responseBuffer.length() > 0 )
	{
		punchIn();
 		int bytesSent = write(fd , responseBuffer.c_str() ,sendSize );
		if (bytesSent <= 0)
		{
			if( bytesSent == -1 && (event.events & EAGAIN  || event.events & EWOULDBLOCK))
			{	
				Logger::log(LC_NOTE , " Minor Error: buffer full or would block!");
				return (false);
			}
			if (bytesSent == 0)
				Logger::log(LC_NOTE , "DONE SENDING, YAHOO!");
			else 
				Logger::log(LC_ERROR, "Unrecoverable socket error, abort process");

			epoll_ctl(fd, EPOLL_CTL_DEL , fd , NULL);
			close(fd);
		}

		responseBuffer = responseBuffer.substr(bytesSent); 
		std::cout << " afterCut responseBuffer = " << std::endl;
		
	}
	
	// std::string testErrorPage = getDefaultErrorPage(status);
	return (true);
	
}

HttpRequest 		&Connection::getHttpRequest()
{
	return  (httpRequest);
}

HttpResponse 		&Connection::getHttpResponse()
{
	return  (httpResponse);
}

