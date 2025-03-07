/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:14:32 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/07 11:32:16 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		CONNECTIONCONTROLLER__HPP
# define	CONNECTIONCONTROLLER__HPP
# include 	<map>
# include	<unistd.h>
# include	<string.h>
# include	<cstring>
# include 	"Connection.hpp"
# include 	"ServerConfig.hpp"
# define	CC_DEF_TIMEOUT_IN_SEC 5

class Connection;

class ConnectionController 
{
	private:

		std::map<int, Connection> 	connections; 
		std::map<int, ServerConfig> servers;

		ConnectionController(ConnectionController const &other);
		ConnectionController &operator=(ConnectionController const &other);

	public:
		ConnectionController();		
		~ConnectionController();

		static int		epollSocket; 

		Connection 		*findConnection(int fd);
		static bool		closeConnection(int fd);
		int				openConnection(int fd, ServerConfig config);

		bool			handleRead(Connection& conn, struct epoll_event& event);
		bool			handleWrite(Connection& conn, struct epoll_event& event , HttpResponse &httpResponse);

		int				addServer(int fd, ServerConfig server);
		ServerConfig	*getServer(int fd);
		std::map<int, ServerConfig> getServers(); 


		static void		setEpollSocket(int epollFd);
		static  int		getEpollSocket();
};

#endif

