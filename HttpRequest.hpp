/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 10:44:57 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	HTTP_REQUEST_HPP
# define	HTTP_REQUEST_HPP
# include   "ServerConfig.hpp"
# include   "HttpResponse.hpp"
# include 	<map>


class ServerConfig;

class HttpRequest
{
	private:

		std::string								method; // GET / POST / DELETE one at a time

		std::string 							path; // string ex, "/" , "/index.html",  "/uploads/"
		std::string 							rawPath; // string ex, "/index.php?name=TEST"

		std::map<std::string, std::string> 		headers;

		size_t									contentLength;
		// GET method submission
		// index.php?name=first&last=blah ,,  split first one from "?" , name , value by "=" , each other by "&"
		std::map<std::string, std::string> 		queryStrings;

		std::string								body;
		std::vector<char>						postBody;


	public:
		HttpRequest();
		HttpRequest(HttpRequest const &other);
		HttpRequest &operator=(HttpRequest const &other);
		~HttpRequest();

		// &response ==> for setting value in case on error
		// ServerConfig
		// requestString = "HTTP1.1 GET /index.php \n\nn\"
		bool parseRequest(HttpResponse &response, ServerConfig &server, std::string requestString);

		
};

#endif 
