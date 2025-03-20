/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 13:46:55 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/19 18:15:22 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef 	SETTINGS_HPP
# define 	SETTINGS_HPP

# define 	WEBS_APP_NAME			"Webserv42/1.0"
# define 	WEBS_MAX_CONNS			100
# define 	WEBS_MAX_EVENTS			1024  // should be change!
# define	WEBS_SCK_TIMEOUT		1000


# define 	CON_RECV_BUFFER_SIZE 	4001
# define 	CON_SOC_TIMEOUT_SECS 	5
# define 	WEBS_MB					1048576 
# define 	WEBS_DEF_MAX_BOD_SIZE	8
# define	WEBS_RESP_SEND_SIZE 	4096

# define	CC_DEF_TIMEOUT_IN_SEC   5


# define	READ_BUFFER_SIZE        1024
# define	CGI_TIMEOUT             10

# define	WEBS_DEBUG_RESPONSE		false
# define 	WEBS_DEBUG_RUN_10_SECS	false	


# define  	LC_DEFAULT 				"\033[36m"
# define  	LC_GREEN  				"\033[32m"
# define  	LC_RED  				"\033[31m"
# define  	LC_YELLOW  				"\033[33m"
# define  	LC_BOLD  				"\033[1m"
# define  	LC_RESET  				"\033[0m"
# define  	LC_ERROR  				"\033[31m\033[1m"
# define  	LC_CLOSE  				"\033[34m"
# define  	LC_DEBUG  				"\033[1m\033[34m"

# define  	LC_SYSTEM  				"\033[34m\033[1m"
# define  	LC_MINOR_SYSTEM  		"\033[34m"
# define  	LC_INFO  				"\033[36m"
# define  	LC_NOTE  				"\033[33m"
# define  	LC_MINOR_NOTE  			"\033[30m"

# define  	LC_REQ_LOG  			"\033[33m"
# define  	LC_RES_OK_LOG  			"\033[32m"
# define  	LC_RES_NOK_LOG  		"\033[31m"
# define  	LC_CON_FAIL  			"\033[31m"
# define  	LC_CONN_LOG  			"\033[35m"


#endif
