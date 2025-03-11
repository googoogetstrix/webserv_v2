/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:15:43 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/10 17:38:55 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		LOGGER_HPP
# define	LOGGER_HPP
# include 	<iostream>

#define  	LC_DEFAULT 			"\033[36m"
#define  	LC_GREEN  			"\033[32m"
#define  	LC_RED  			"\033[31m"
#define  	LC_YELLOW  			"\033[33m"
#define  	LC_BOLD  			"\033[1m"
#define  	LC_RESET  			"\033[0m"
#define  	LC_ERROR  			"\033[31m\033[1m"
#define  	LC_CLOSE  			"\033[34m"
#define  	LC_DEBUG  			"\033[1m\033[34m"

#define  	LC_SYSTEM  			"\033[32m\033[1m"
#define  	LC_MINOR_SYSTEM  	"\033[32m"
#define  	LC_INFO  			"\033[33m"
#define  	LC_NOTE  			"\033[33m"
#define  	LC_MINOR_NOTE  		"\033[30m"

#define  	LC_REQ_LOG  		"\033[34m"
#define  	LC_RES_OK_LOG  		"\033[32m"
#define  	LC_RES_NOK_LOG  	"\033[31m"
#define  	LC_CON_FAIL  		"\033[31m"



class Logger
{
    private:
		Logger();
		Logger(Logger const &other);
		Logger &operator=(Logger const &other);
		~Logger();

	public:
		//void log(int output, std::string color, std::string message);
		static void log(std::string color, std::string  message, ...);
		static std::string getTimestamp();
};

#endif 
