/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:12:40 by bworrawa          #+#    #+#             */
/*   Updated: 2025/02/18 11:44:06 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		SERVERCONFIG_HPP
# define	SERVERCONFIG_HPP
# include 	<iostream>
# include 	<vector>
# include 	<sstream>

typedef struct s_path
{
	bool						autoindex;  // on/off
	std::string 				index;  	// tours1.html;
	std::vector<std::string> 	allow_methods; // GET POST PUT HEAD;
	std::string					root; // ./;
	std::string					cgi_path ; // /usr/bin/python3 /bin/bash;
	std::vector<std::string>	cgi_ext ; // .py .sh;	
} t_path;


class ServerConfig
{
	private:
		int							port;
		std::string					server_name;
		std::string					root;
		std::string					index;
		std::vector<std::string>	allowed_methods;
		std::vector<s_path>			paths; // aka locations /red




	public:
		ServerConfig();
		ServerConfig(ServerConfig const &other);
		ServerConfig &operator=(ServerConfig const &other);
		~ServerConfig();


		void dummy(void);
		void dummy2(void);


		int			getPort(void);
		std::string	getNick();


		
};

#endif
