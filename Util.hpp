/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:35:29 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/12 11:03:15 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef		UTIL_HPP
# define 	UTIL_HPP 
# include 	<iostream>
# include 	<vector>

class Util
{
	public: 
		static	std::string trim(std::string&);
		static	std::string trim(std::string);
		static	std::string	toString(int i);
		static	std::string	toString(size_t i);
		static	std::string	replaceAll(std::string src, std::string const &from , std::string const &to);

		static	int toInt(std::string);
		static	size_t toSizeT(std::string);

		static 	int	substrPos(std::string const haystack, std::string const needle);
		static	size_t charactersMatched(std::string find, std::string path);
		static  std::string	 extractFileName(std::string const path, bool containsDot=true);
		static 	std::string  getFileExtension(std::string const &filePath);

		static  bool	 hasTrailingSlash(std::string const path);


		static  bool 	 strInContainer(std::string const &str, std::vector<std::string> v);
};

#endif
