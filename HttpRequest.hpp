/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:10:12 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/07 18:05:53 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef 	HTTP_REQUEST_HPP
# define	HTTP_REQUEST_HPP
# include	<map>
# include	<string>
# include   "ServerConfig.hpp"
# include   "HttpResponse.hpp"
# include   "Logger.hpp"


class ServerConfig;
class HttpResponse;

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
		std::string								rawQueryString;

		std::string								body;


	public:
		HttpRequest();
		HttpRequest(HttpRequest const &other);
		HttpRequest &operator=(HttpRequest const &other);
		~HttpRequest();

		const std::string &getMethod() const;
		const std::string &getPath() const;
		const std::string &getRawPath() const;
		const std::map<std::string, std::string> &getHeader() const;
		size_t getContentLength() const;
		const std::map<std::string, std::string> &getqueryStrings() const;
		const std::string &getBody() const;

		bool setMethod(std::string methodStr);
		bool setPath(std::string pathStr);
		bool setRawPath(std::string rawPathStr);
		bool setHeader(std::string name, std::string value , bool overwriteExisting=false);
		bool setContentLength(size_t contentLengthVal);
		bool setQueryString(std::string key, std::string value);
		bool setBody(std::string bodyStr);

		std::string getRawQueryString() const; 


		// &response ==> for setting value in case on error
		// ServerConfig
		// requestString = "HTTP1.1 GET /index.php \n\nn\"
		bool parseRequestHeaders(HttpResponse &response, ServerConfig &server, std::string requestString);



		void  debug();
		

		
};

#endif 
