/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:35:29 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 14:46:50 by bworrawa         ###   ########.fr       */
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

};

#endif
