/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:37:19 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/05 11:19:35 by bworrawa         ###   ########.fr       */
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
