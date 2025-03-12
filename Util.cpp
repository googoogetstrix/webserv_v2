/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:37:19 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/12 11:03:26 by bworrawa         ###   ########.fr       */
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


std::string	 Util::extractFileName(std::string const path, bool containsDot)
{
    size_t pos = path.find_last_of("/");

    if(pos == std::string::npos)
        return path;
    else
    {
        std::string  temp = path.substr(pos + 1);
        if(containsDot)
            return (temp.find(".") != std::string::npos)? temp : "" ;
        else 
            return (temp);
    } 
        
}
bool	 Util::hasTrailingSlash(std::string const path)
{
    return  (path[ path.length() - 1] == '/');
}


bool 	Util::strInContainer(std::string const &str, std::vector<std::string> v)
{
	for( std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
	{
		if( str == *it)
			return (true);		
	}
	return false;
}


std::string	Util::replaceAll(std::string str, std::string const &from , std::string const &to)
{
    if(from.empty())
        return str;
    std::string src = str; 
   
    size_t pos = 0;
    while ((pos = src.find(from, pos)) != std::string::npos) {
        src.replace(pos, from.length(), to);
        pos += to.length();
    }
    return src;
}


std::string Util::getFileExtension(std::string const &filePath)
{
    size_t dotPos = filePath.find_last_of(".");
	if (dotPos != std::string::npos)
		return (filePath.substr(dotPos));
	return "";
}

