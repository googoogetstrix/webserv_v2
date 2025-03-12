/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/12 11:15:23 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	HTTP_RESPONSE_HPP
# define	HTTP_RESPONSE_HPP
# include 	<map>
# include 	<sys/socket.h>
# include	<sys/stat.h>
# include	<sys/wait.h>
# include	<fstream>
# include 	<iostream>
# include 	<vector>
# include 	<sstream>
# include	<string>
# include 	<unistd.h>
# include	<dirent.h>
# include	<errno.h>
# include	<signal.h>
# include 	"Util.hpp"
# include	"HttpRequest.hpp"
# include	"ServerConfig.hpp"
# include	"RouteConfig.hpp"

class ServerConfig;

class HttpRequest;

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
		static std::string					getErrorPage(int statusCode, ServerConfig server);
		static std::string					getDefaultErrorPage(int statusCode);
		static std::string					getMimeType(const std::string & extension);
		bool								getStaticFile(std::string const &localPath);

		bool 								response(int clientSocket);

		void								debug() const;
		
		bool								generateDirectoryListing(const std::string& path);

		void								processPythonCGI(const HttpRequest request, ServerConfig server, RouteConfig route);

};
#endif 
