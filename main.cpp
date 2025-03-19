/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:24:58 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/19 16:18:29 by bworrawa         ###   ########.fr       */
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




int main(int argc, char **argv)
{
	try {
		if (argc > 2)
			throw std::invalid_argument("Invalid arguments, use webserv [configuration_file]");
		std::string config = (argc == 1) ? WS_DEF_CONFIG_FILE : std::string(argv[1]);
		signal(SIGPIPE, SIG_IGN);
		Webserv webserv(config);

		webserv.run();
	}
	catch(std::exception &e)
	{
		std::cout << WS_COL_BOLD << WS_COL_RED << "Error: " << e.what() << WS_COL_RESET << std::endl;
	}
	


	return (0);
}
