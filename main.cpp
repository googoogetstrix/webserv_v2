/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:24:58 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/15 10:11:05 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <exception>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "Webserv.hpp"
#define  WS_COL_RED  			"\033[31m"
#define  WS_COL_BOLD  			"\033[1m"
#define  WS_COL_RESET  			"\033[0m"
#define	 WS_DEF_CONFIG_FILE		"./configs/default.conf"
#define	 WS_MAX_SOCKETS			50	



void sigpipe_handler(int signo)
{
	(void)signo;
}


int main(int argc, char **argv)
{
	// std::string haystack = "Hello World";
	// std::string needle = "XWor";
	// int  pos = Util::substrPos(haystack , needle);
	// std::cout << "pos = " << pos << std::endl ;
	// return (0);

	// std::string str = std::string("./wwwroot/www1/uploads/del_me");
	// bool out = HttpResponse::checkFileAvailibity( str );
	// std::cout << " out = " << out << std::endl;

	// str = std::string("./wwwroot/www1/uploads/s2.txt");
	// out = HttpResponse::checkFileAvailibity( str );
	// std::cout << " out = " << out << std::endl;

	// return 0;


	try {
		if (argc > 2)
			throw std::invalid_argument("Invalid arguments, use webserv [configuration_file]");
		std::string config = (argc == 1) ? WS_DEF_CONFIG_FILE : std::string(argv[1]);

		Webserv webserv(config);


		// /*** TEST CODES ***/
		// std::vector<ServerConfig> servers = webserv.getServerConfigs();
		// ServerConfig  server = servers[0];
		// RouteConfig *route = server.findRoute("/");
		// std::cout << " RETURNED ROUTE = " << route->getPath() << std::endl;
		// Logger::log(LC_RED, " EXIT IN MAIN" );
		// throw std::runtime_error("ME FAIL");
		// /*** TEST CODES ***/


		webserv.run();
		signal(SIGPIPE, sigpipe_handler);
		
	}
	catch(std::exception &e)
	{
		std::cout << WS_COL_BOLD << WS_COL_RED << "Error: " << e.what() << WS_COL_RESET << std::endl;
	}
	


	return (0);
}
