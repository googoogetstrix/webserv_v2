/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:56:59 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/03 19:27:10 by bworrawa         ###   ########.fr       */
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


HttpResponse::HttpResponse()
{

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


std::string HttpResponse::serialize() const
{
	std::ostringstream  	ss;
	ss << "HTTP/1.1 " << status << " " << getStatusText(status) << "\r\n";
	for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		ss << trim(it->first) << ": " << it->second << "\r\n";
	ss << "\r\n\r\n" << body;
	return ss.str();
	
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
