/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:25:45 by bworrawa          #+#    #+#             */
/*   Updated: 2025/02/13 10:27:55 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{

}
ServerConfig::ServerConfig(ServerConfig const &other)
{
	if(this != &other)
		*this = other;

}
ServerConfig &ServerConfig::operator=(ServerConfig const &other)
{
	if(this != &other)
	{
		port = other.port;
		server_name = other.server_name;
		root = other.root;
		index = other.index;
		allowed_methods = other.allowed_methods;
		paths  = other.paths;
	}	
		
	return (*this);
}

ServerConfig::~ServerConfig()
{

}

void ServerConfig::dummy(void)
{
	port = 4343;
	server_name = "localhost"; /// www.example.com
	root = "./webroot/www1/";
	index = "index.html";
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
	// paths = '' // nah

}

void ServerConfig::dummy2(void)
{
	port = 4444;
	server_name = "localhost2";
	root = "./webroot/www2/";
	index = "index.html";
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
	// paths = '' // nah

}

int	ServerConfig::getPort(void)
{
	return port;
}

std::string ServerConfig::getNick()
{
	std::stringstream ss;

	if (!server_name.empty())
		ss << server_name;
	else
		ss << "server";
	
	return (ss.str());
}
