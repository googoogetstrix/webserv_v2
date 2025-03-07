/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:24:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/06 15:57:39 by bworrawa         ###   ########.fr       */
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
	catch (std::exception &exception)
	{

	}

	return (true);
	
}
bool 	Connection::ready(struct epoll_event &event, HttpResponse &httpResponse)
{
	responseBuffer = httpResponse.serialize();
	bool   res = handleWrite(epollSocket , event);
	// if(!res)
	// 	std::cout << " RES = " << res << std::endl;
	return (res);

}

bool	Connection::needsToWrite()
{
	return (!responseBuffer.empty());
}


bool 	Connection::handleWrite( int epoll_fd, struct epoll_event &event)
{
	std::cout << "\t\t\t*** handleWrite()" << std::endl;
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
			Logger::log(LC_RED, " bytesSent = %d" , bytesSent); 
			if( bytesSent == -1 && (event.events & EAGAIN  || event.events & EWOULDBLOCK))
			{	
				Logger::log(LC_NOTE , " Minor Error: buffer full or would block!");
				return (false);
			}
			if (bytesSent == 0)
			{
				Logger::log(LC_NOTE , "DONE SENDING, YAHOO!");

				close(event.data.fd);
				epoll_ctl(epoll_fd , EPOLL_CTL_DEL , event.data.fd , NULL);
				Logger::log(LC_GREEN , "Socket#%d Done writing, closing socket happily.", event.data.fd);

				return (true);
			}
				
			else 
				Logger::log(LC_ERROR, "Unrecoverable socket error, abort process");

			ConnectionController::closeConnection(fd);
		}
		size_t compareSize = static_cast<size_t>(bytesSent);

		compareSize = compareSize < responseBuffer.length() ? bytesSent = responseBuffer.length() : compareSize;

		responseBuffer = responseBuffer.substr(compareSize); 
		
		
	}
	
	// std::string testErrorPage = getDefaultErrorPage(status);
	return (true);
	
}


