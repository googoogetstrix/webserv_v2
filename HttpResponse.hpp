/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nusamank <nusamank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 10:38:23 by nusamank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	HTTP_RESPONSE_HPP
# define	HTTP_RESPONSE_HPP
# include 	<map>
# include 	<sys/socket.h>
# include	<fstream>
# include 	<iostream>
# include 	<vector>
# include 	<sstream>
# include	<string>
# include 	<unistd.h>
# include 	"Util.hpp"
# include	"HttpRequest.hpp"
# include	"ServerConfig.hpp"
# include	"RouteConfig.hpp"

class HttpResponse
{
	private:
		int								status;
		std::map<std::string,std::string>	headers;
		std::string							body;

	public:
		HttpResponse();
		HttpResponse(HttpResponse const &other);
		HttpResponse &operator=(HttpResponse const &other);
		~HttpResponse();
		
		std::string 						getHeader(std::string name) const;
		bool 								setHeader(std::string name, std::string value , bool overwriteExisting=false);
		std::map<std::string, std::string> 	&getHeaders(void);

		bool								setStatus(int statusCode);
		int									getStatus();

		std::string							getBody() const;
		bool								setBody(std::string body);

		std::string 						serialize();
		void								clear();
		
		static std::string					getStatusText(int statusCode);
		static std::string					getDefaultErrorPage(int statusCode);
		static std::string					getStaticFile(HttpRequest const &request, ServerConfig &server, RouteConfig &route);

		bool 								response(int clientSocket);
};
#endif 
