/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:25:45 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/03 16:17:52 by bworrawa         ###   ########.fr       */
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
