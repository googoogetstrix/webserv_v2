/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/03 19:19:10 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	HTTP_RESPONSE_HPP
# define	HTTP_RESPONSE_HPP
# include 	<map>
# include 	<sys/socket.h>
# include 	<iostream>
# include 	<vector>
# include 	<sstream>
# include 	<unistd.h>

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
		std::map<std::string, std::string> 	&getHeaders(void) const;

		bool								setStatus(int statusCode);
		int									getStatus();

		std::string							getBody() const;
		bool								setBody(std::string body);

		std::string 						serialize() const;
		void								clear();
		
		static std::string					getStatusText(int statusCode);
		static std::string					getDefaultErrorPage(int statusCode);

		bool 								response(int clientSocket);
};
#endif 
