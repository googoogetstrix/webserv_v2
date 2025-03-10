/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:25:45 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/10 14:47:10 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{

}
HttpRequest::HttpRequest(HttpRequest const &other)
{
	if(this != &other)
		*this = other;
}
HttpRequest &HttpRequest::operator=(HttpRequest const &other)
{
	if(this != &other)
		*this = other;
	return (*this);
}

HttpRequest::~HttpRequest()
{

}

const std::string &HttpRequest::getMethod() const { return method; }
const std::string &HttpRequest::getPath() const { return path; }
const std::string &HttpRequest::getRawPath() const { return rawPath; }
const std::map<std::string, std::string> &HttpRequest::getHeader() const { return headers; }
size_t HttpRequest::getContentLength() const { return contentLength; }
const std::map<std::string, std::string> &HttpRequest::getqueryStrings() const { return queryStrings; }
const std::string &HttpRequest::getBody() const { return body; }

bool HttpRequest::setMethod(std::string methodStr)
{
	method = methodStr;
	return true;
}

bool HttpRequest::setPath(std::string pathStr)
{
	path = pathStr;
	return true;
}

bool HttpRequest::setRawPath(std::string rawPathStr)
{
	rawPath = rawPathStr;
	return true;
}

bool HttpRequest::setHeader(std::string name, std::string value , bool overwriteExisting)
{
	std::map<std::string, std::string>::const_iterator pos = headers.find(name);
	if (pos != headers.end() && !overwriteExisting)
		return false;
	else if (pos != headers.end() && overwriteExisting)
	{
		while (pos != headers.end())
		{
			name += " ";
			pos = headers.find(name);

		}
		headers[name] = value;
		return true;
	}
	else
	{
		headers[name] = value;
		return true;
	}
}

bool HttpRequest::setContentLength(size_t contentLengthVal)
{
	contentLength = contentLengthVal;
	return true;
}

bool HttpRequest::setQueryString(std::string key, std::string value)
{
	queryStrings[key] = value;
	return true;
}

bool HttpRequest::setBody(std::string bodyStr)
{
	body = bodyStr;
	return true;
}


int HttpRequest::preprocessContentLength(std::string requestString)
{
	int	return_len;
	std::string			line ;
	std::istringstream 	stream(requestString);
	while ( std::getline(stream , line)  && line != "\r")
	{
		 if(line.find("Content-Length:") == 0)
		 {
			std::istringstream len_stream(line.substr(15));
			if(!(len_stream >> return_len))
				return -1;
			return return_len;
		 }
	}
	return -1; 

}

bool HttpRequest::parseRequestHeaders(ServerConfig server, std::string requestString)
{

	// std::cout << "CALLING httpRequest::parseRequestHeaders() " << std::endl;
	// std::cout << "raw param: " << requestString << std::endl;
	std::istringstream requestStream(requestString);
	std::string line;

	if (std::getline(requestStream, line))
	{
		std::istringstream lineStream(line);
		std::string methodStr;
		std::string rawPathStr;
		std::string httpVersion;
		if (!(lineStream >> methodStr >> rawPathStr >> httpVersion))
		{
			throw RequestException(400, "Bad Request");
			// response.setStatus(400);
			return false;
		}
		{	// prevent url injection with ".." which allows the web server to go further back than webroots
			size_t dotdotPos = rawPathStr.find("..");
			if (dotdotPos != std::string::npos)
			{
				throw RequestException(400, "Bad Request");
				// response.setStatus(400);
				return false;
			}
		}

		setMethod(methodStr);
		setRawPath(rawPathStr);
		size_t pos = rawPathStr.find('?');
		if (pos != std::string::npos)
		{
			setPath(rawPathStr.substr(0, pos));
			rawQueryString = rawPathStr.substr(pos + 1);
			std::istringstream queryStream(rawQueryString);
			std::string keyValuePair;
			while (std::getline(queryStream, keyValuePair, '&'))
			{
				size_t equalPos = keyValuePair.find('=');
				if (equalPos != std::string::npos)
					setQueryString(keyValuePair.substr(0, equalPos), keyValuePair.substr(equalPos + 1));
				else
					setQueryString(keyValuePair, "");
			}
		}
		else
			setPath(rawPathStr);
	}
	while (std::getline(requestStream, line) && line != "r")
	{
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			std::string name = line.substr(0, colonPos);
			std::string value = Util::trim(line.substr(colonPos + 1));
			setHeader(name, value, false);
		}
	}
	if (headers.find("Content-Length") != headers.end())
	{
		size_t contentLengthVal = Util::toInt(headers["Content-Length"].c_str());
		if (contentLengthVal > server.getClientMaxBodySize())
		{
			throw RequestException(400, "Content too large");
			// response.setStatus(413);
			return false;
		}
		setContentLength(contentLengthVal);
		std::string bodyStr(contentLength, '\0');
        requestStream.read(&bodyStr[0], contentLength);
        setBody(bodyStr);
	}
	return true;
}

std::string HttpRequest::getRawQueryString() const
{
	return rawQueryString; 
}


void HttpRequest::debug()
{
	std::cout << "========================\nHttpRequest\n========================" << std::endl;
	std::cout << " - method:\t\t" << method << std::endl;
	std::cout << " -  path:\t\t" << path << std::endl;
	std::cout << " - rawPath:\t" << rawPath << std::endl;
	std::cout << " - rawQueryString:\t" << rawQueryString << std::endl;
	std::cout << " - contentLength:\t" << contentLength << std::endl;
	std::cout << " - body:\t\t" << body << std::endl;
	std::cout << " - header:" << std::endl;

	for(std::map<std::string, std::string>::iterator  it = headers.begin(); it != headers.end() ; ++it)
	{
		std::cout << "\t" << it->first << "\t: " << it->second << std::endl;
	}

}

std::string HttpRequest::getHeader(std::string const str)
{
	if(headers.find(str) == headers.end())
		return "";

	return headers.at(str);

}
