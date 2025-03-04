/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:56:59 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 13:51:33 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"


static std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        ++start;
    }

    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}


HttpResponse::HttpResponse():status(200)
{
	setHeader("Content-Type", "text/html");
}
HttpResponse::HttpResponse(HttpResponse const &other)
{
	if (this != &other)
		*this = other;
}
HttpResponse &HttpResponse::operator=(HttpResponse const &other)
{
	if (this != &other)
		*this = other;
	return (*this);
}
HttpResponse::~HttpResponse()
{

}


std::string HttpResponse::getStatusText(int statusCode)
{
	(void) statusCode;
	return "TO BE IMPLEMENTED";
}


std::string HttpResponse::serialize()
{
	std::ostringstream  	oss;
	std::stringstream 		ss;
	oss << "HTTP/1.1 " << status << " " << getStatusText(status) << "\r\n";
	ss << body.size();
	setHeader("Content-Length", ss.str(), true);

	for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		oss << trim(it->first) << ": " << it->second << "\r\n";
	// single set of \r\n since the header already sent the first set
	oss << "\r\n" << body;

	std::cout << "===================================" << std::endl;
	std::cout << "response" << std::endl;
	std::cout << "===================================" << std::endl;
	std::cout << oss.str() << std::endl;
	std::cout << "===================================" << std::endl;
	return oss.str();
	
}

bool HttpResponse::response(int socket_id)
{
	
	std::string 			wholeResponse = serialize();
	size_t					bytesSent = 0;

	while( bytesSent < wholeResponse.length())
	{
 		int sent = write(socket_id , wholeResponse.c_str() + bytesSent, wholeResponse.length() - bytesSent);
		if(sent <= 0)
			return false;
		bytesSent += sent;
	}
	return (true);
}

bool HttpResponse::setStatus(int statusCode)
{
	status = statusCode; 
	return (true);
}
bool HttpResponse::setBody(std::string bodyString)
{
	body = bodyString;
	return (true);
}


bool HttpResponse::setHeader(std::string name, std::string value , bool overwriteExisting)
{
	(void)overwriteExisting;
	headers[name] = value;
	return (true);
}
