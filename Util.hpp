/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nusamank <nusamank@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:35:29 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 23:08:29 by nusamank         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		UTIL_HPP
# define 	UTIL_HPP 
# include 	<iostream>

class Util
{
	public: 
		static	std::string trim(std::string&);
		static	std::string trim(std::string);
		static	std::string	toString(int i);
		static	std::string	toString(size_t i);
		static	int toInt(std::string);

};

#endif
