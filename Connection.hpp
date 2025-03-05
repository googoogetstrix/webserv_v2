/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:17:25 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 10:44:38 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	CONNECTION_HPP
# define	CONNECTION_HPP
# define 	CONN_FD_FLAG_NONBLOCK 	1
# include	<ctime>
# include	<fcntl.h>
# include	<stdexcept>
# include	"ServerConfig.hpp"

class Connection 
{
	private:

		int					fd;	
		time_t				lastActive; 
		ServerConfig		config;

		std::string			requestBuffer;
		std::string 		responseBuffer;

		void 			setNonBlock();
	
	public:
		Connection();
		Connection(int fd, ServerConfig config);
		Connection(Connection const &other);
		Connection &operator=(Connection const other);
		~Connection();

		time_t			getLastActive() const ;
		int 			getFd() const;
		std::string		getRequestBuffer() const;
		std::string 	getResponseBuffer() const;

		bool 			setLastActive(time_t);
		bool 			setFd(int fd);

		void			punchIn(void);



}; 

#endif
