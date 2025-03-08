/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:37:19 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/08 19:34:23 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include 	"Util.hpp"
#include 	<iostream>
#include 	<sstream>

std::string Util::trim(std::string& str)
{
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        ++start;
    }

    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}

std::string Util::trim(std::string str)
{
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        ++start;
    }

    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}
std::string	Util::toString(int i)
{
	std::stringstream 		ss;
	ss << i;
	return (ss.str());
}

std::string	Util::toString(size_t i)
{
	std::stringstream 		ss;
	ss << i;
	return (ss.str());

}

int Util::toInt(std::string s)
{
    std::stringstream       ss(s);
    int i;
    ss >> i;
    return i;
}
size_t Util::toSizeT(std::string s)
{
    std::stringstream       ss(s);
    size_t i;
    ss >> i;
    return i;
}

int	Util::substrPos(const std::string haystack,const  std::string needle)
{
	size_t cit = haystack.find(needle);
	if (cit == std::string::npos)
		return (-1);		
	return (cit);

}



size_t Util::charactersMatched(std::string find, std::string path)
{
	size_t matched = 0 ;
	size_t maxCheck = find.length() < path.length() ? find.length() :path.length(); 
	size_t i;
	for(i = 0; i <maxCheck; i++)
		if(find[i] != path[i])
			break;
	matched = i;
	return (matched);
}
