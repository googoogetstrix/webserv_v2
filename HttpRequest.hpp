/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/03 16:19:09 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	HTTP_REQUEST_HPP
# define	HTTP_REQUEST_HPP
# include   "ServerConfig.hpp"

class ServerConfig;

class HttpRequest
{
	private:

		std::string		method;
		


	public:
		HttpRequest();
		HttpRequest(HttpRequest const &other);
		HttpRequest &operator=(HttpRequest const &other);
		~HttpRequest();

		
};

#endif 
