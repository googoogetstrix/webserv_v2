/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestOkException.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 13:07:53 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/09 15:41:59 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		REQUESTOKEXCEPTION_HPP
# define	REQUESTOKEXCEPTION_HPP
# include 	<exception>
# include 	<iostream>
# include 	<sstream>
# include	"HttpResponse.hpp"

class ResponseOkException: public std::exception
{
	private:
		int			statusCode;
		std::string	message;
		HttpRequest	*httpRequest;



	public:
		ResponseOkException();
		ResponseOkException(int statusCode, std::string const &message , HttpRequest &httpRequest);
		ResponseOkException(ResponseOkException const &other);
		ResponseOkException &operator=(ResponseOkException const &other);

		~ResponseOkException() throw ();
		virtual const char* what() const throw();
		
		int	getCode() const;
		std::string	getMessage() const;
		HttpRequest	&getHttpRequest() const;

};

#endif
