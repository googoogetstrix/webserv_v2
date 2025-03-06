/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionController.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:14:32 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/06 10:50:55 by bworrawa         ###   ########.fr       */
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



class ConnectionController 
{
	private:

		std::map<int, Connection> 	connections; 
		size_t						timeoutInSecs;

		std::map<int, ServerConfig> servers;

		ConnectionController(ConnectionController const &other);
		ConnectionController &operator=(ConnectionController const &other);



	public:
		ConnectionController();
		ConnectionController(int timeout);
		~ConnectionController();


		Connection *findConnection(int fd);
		bool		removeConnection(int fd);
		int			addConnection(int fd, ServerConfig config);

		bool		handleRead(Connection& conn, struct epoll_event& event);
		bool		handleWrite(Connection& conn, struct epoll_event& event);

		int							addServer(int fd, ServerConfig server);
		ServerConfig				*getServer(int fd);
		std::map<int, ServerConfig> getServers(); 


};

#endif

