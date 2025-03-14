/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 17:17:25 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/13 18:15:29 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	CONNECTION_HPP
# define	CONNECTION_HPP
# define 	CONN_FD_FLAG_NONBLOCK 	1
# include	<ctime>
# include 	<cstdio>
# include	<fcntl.h>
# include	<stdexcept>
# include	<string>
# include 	<errno.h>
# include 	<sys/epoll.h>
# include 	<sstream>
# include 	<unistd.h>

# include	"ServerConfig.hpp"
# include	"HttpRequest.hpp"
# include	"HttpResponse.hpp"
# include	"ConnectionController.hpp"
# include	"Logger.hpp"

# define 	CON_RECV_BUFFER_SIZE 	4001
# define 	CON_SOC_TIMEOUT_SECS 	5
# define 	WEBS_MB					1048576 
# define 	WEBS_DEF_MAX_BOD_SIZE	8
# define	WEBS_RESP_SEND_SIZE 	4096

class Connection 
{
	private:

		int					fd;	
		time_t				expiresOn; 
		ServerConfig		serverConfig;
		bool				isReady;
		int					bodyLength;
		std::string			boundary;

		std::string			requestBuffer;
		std::string 		responseBuffer;
		std::vector<char>	rawPostBody;
		bool				headerIsCompleted;
		bool				requestIsCompleted;
		int					epollSocket;
		void 				setNonBlock();
		size_t 				contentLength;


	
	public:
		Connection();
		Connection(int fd, ServerConfig config);
		Connection(Connection const &other);
		Connection &operator=(Connection const other);
		~Connection();

		time_t				getExpiresOn() const ;
		int 				getSocket() const;
		std::string			getRequestBuffer() const;
		std::string 		getResponseBuffer() const;
		bool 				getHeaderIsComplete() const;
		bool 				getRequestIsComplete() const;
		void 				setRequestIsComplete(bool newValue);

		std::vector<char>	&getRawPostBody();

		bool 				setExpiresOn(time_t);
		bool 				setFd(int fd);

		void				punchIn(void);

		void				setHeaderIsComplete(bool newValue);
		bool				isHeaderComplete(); 

		bool				appendRawPostBody(char *, size_t bytesRead);
		bool				appendRequestBuffer(char *buffer, size_t length);

		bool				processRequest(HttpRequest &httpRequest);

		bool				ready(HttpResponse &httpResponse, bool sendAsWell=false);
		bool				getIsReady() const;
		void				setIsReady(bool newValue) ;
		bool				needsToWrite();
//		bool				handleWrite(int epoll_fd, struct epoll_event &event);

		size_t				truncateResponseBuffer(size_t bytes);

		bool				isExpired(time_t comp) const;



		ServerConfig		&getServerConfig();

		void 	setContentLength(int i);
		size_t				getContentLength();

		void				debugPostBody();
		void 				debug();
		void 				clear();


				

		class ParseRequestException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		
		
}; 

#endif
