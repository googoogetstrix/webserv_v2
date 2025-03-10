/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestOkException.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 13:14:53 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/09 13:45:13 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseOkException.hpp"



ResponseOkException::ResponseOkException(ResponseOkException const &other)
{
	statusCode = other.statusCode;
	message = other.message;
    httpRequest = other.httpRequest;
}
ResponseOkException &ResponseOkException::operator=(ResponseOkException const &other)
{
	statusCode = other.statusCode;
	message = other.message;
    httpRequest = other.httpRequest;
	return *(this);
}



ResponseOkException::ResponseOkException():statusCode(0),message( std::string("Something Wrong"))
{
	
}
ResponseOkException::ResponseOkException(int statusCode, std::string const &message, HttpRequest &httpRequest):statusCode(statusCode), message(message), httpRequest(&httpRequest)
{

}
ResponseOkException::~ResponseOkException() throw ()
{


}
const char* ResponseOkException::what() const throw()
{
	
	return message.c_str();

}

int			ResponseOkException::getCode() const
{
	return (statusCode);
}

std::string	ResponseOkException::getMessage() const
{
	return (message);
}


HttpRequest  &ResponseOkException::getHttpRequest() const
{
    return *httpRequest;
}
