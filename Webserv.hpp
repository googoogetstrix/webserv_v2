/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:15:43 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/08 19:35:13 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		WEBSERV_HPP
# define	WEBSERV_HPP
# include 	<iostream>
# include 	<vector>
# include 	<algorithm>
# include 	<set>
# include 	<sys/socket.h>
# include	<netinet/in.h>
# include	<sys/epoll.h>
# include 	<fcntl.h>
# include 	<unistd.h>
# include 	<cstring>
# include 	<cerrno>
# include 	"ServerConfig.hpp"
# include 	"HttpRequest.hpp"
# include 	"HttpResponse.hpp"
# include 	"ConnectionController.hpp"
# include 	"ConfigParser.hpp"

# define 	WEBS_MAX_FDS			10240
# define 	WEBS_MAX_CONNS			100
# define 	WEBS_MAX_EVENTS			10240  // should be change!
# define	WEBS_SCK_TIMEOUT		1000

# define 	WEBS_MB					1048576 
# define 	WEBS_DEF_MAX_BOD_SIZE	8 // 8MB 
class ServerConfig; 
class ConnectionController;

class Webserv
{
	

	private:
		std::vector<ServerConfig>	servers;
		std::string					config_file;
		std::vector<int>			server_fds;
		
		Webserv();
		Webserv(Webserv const &other);
		Webserv &operator=(Webserv const &other);


		size_t	parseConfig(std::string config_file);


		bool	setupSockets(ConnectionController &cc);

		bool	findServerFromRequest(HttpRequest &req, ServerConfig &sc);

		bool	isServerFd(int fd);



	public:
		
		~Webserv();
		Webserv(std::string config_file);
		std::set<int> getListeningPorts();

		int 						run(void);
		std::vector<ServerConfig> 	getServerConfigs();
		

};

#endif
