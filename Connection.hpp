/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:17:25 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 16:23:30 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	CONNECTION_HPP
# define	CONNECTION_HPP
# define 	CONN_FD_FLAG_NONBLOCK 	1
# include	<ctime>
# include	<fcntl.h>
# include	<stdexcept>
# include	<string>
# include 	<errno.h>
# include 	<sys/epoll.h>
# include 	<sstream>


# include	"ServerConfig.hpp"
# include	"HttpRequest.hpp"
# include	"HttpResponse.hpp"
# define 	CON_RECV_BUFFER_SIZE 	4001

class Connection 
{
	private:

		int					fd;	
		time_t				lastActive; 
		ServerConfig		config;

		std::string			requestBuffer;
		std::string 		responseBuffer;
		std::vector<char>	rawPostBody;
		bool				headerIsCompleted;

		void 				setNonBlock();

		HttpRequest			httpRequest;		
		HttpResponse		httpResponse;		
	
	public:
		Connection();
		Connection(int fd, ServerConfig config);
		Connection(Connection const &other);
		Connection &operator=(Connection const other);
		~Connection();

		time_t				getLastActive() const ;
		int 				getFd() const;
		std::string			getRequestBuffer() const;
		std::string 		getResponseBuffer() const;

		bool 				setLastActive(time_t);
		bool 				setFd(int fd);

		void				punchIn(void);

		void				setHeaderIsComplete(bool newValue);
		bool				isHeaderComplete(); 

		bool				appendRawPostBody(char *, size_t bytesRead);
		bool				appendRequestBuffer(std::string str);

		bool				processRequestHeader();
		bool				processRequest();




}; 

#endif
