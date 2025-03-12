/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:56:59 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/12 11:08:54 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HttpResponse.hpp"
#include "Webserv.hpp"


HttpResponse::HttpResponse():status(200)
{
	setHeader("Content-Type", "text/html");
}
HttpResponse::HttpResponse(HttpResponse const &other)
{
	if (this != &other)
		*this = other;
}
HttpResponse &HttpResponse::operator=(HttpResponse const &other)
{
	if (this != &other)
		*this = other;
	return (*this);
}
HttpResponse::~HttpResponse()
{

}

std::string HttpResponse::getHeader(std::string name) const
{
	std::map<std::string, std::string>::const_iterator pos = headers.find(name);
	if (pos != headers.end())
		return pos->second;
	else
		return "";
}

bool HttpResponse::setHeader(std::string name, std::string value , bool overwriteExisting)
{
	std::map<std::string, std::string>::const_iterator pos = headers.find(name);
	if (pos != headers.end() && !overwriteExisting)
		return false;
	else if (pos != headers.end() && !overwriteExisting)
	{
		while (pos != headers.end())
		{
			name += " ";
			pos = headers.find(name);

		}
		headers[name] = value;
		return true;
	}
	else
	{
		headers[name] = value;
		return true;
	}
}

std::map<std::string, std::string> 	&HttpResponse::getHeaders(void)
{
	return headers;
}


int	HttpResponse::getStatus()
{
	return status;
}

bool	HttpResponse::setStatus(int newStatusCode)
{
	if (newStatusCode < 100 || newStatusCode > 599)
		return false;
	status = newStatusCode;
	return true;
}

std::string	HttpResponse::getBody() const
{
	return body;
}


bool	HttpResponse::setBody(std::string newBody)
{
 	body = newBody; 
	return true;
}

std::string HttpResponse::getStatusText(int statusCode)
{
	switch (statusCode)
	{
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "No Content";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 304:
			return "Not Modified";
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 408:
			return "Request Timeout";
		case 413:
			return "Payload Too Large";
		case 500:
			return "Internal Server Error";
		case 503:
			return "Service Unavailable";

		default:
			return "Unknown Status";
	}
}

std::string HttpResponse::getErrorPage(int statusCode, ServerConfig server)
{
	std::map<int, std::string> errorPages = server.getErrorPages();
	std::map<int, std::string>::iterator it = errorPages.find(statusCode);
	if (it != errorPages.end())
	{
		std::ifstream file(it->second.c_str());
		if (!file.is_open())
			return getDefaultErrorPage(statusCode);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string errorPage = buffer.str();
		return errorPage;
	}
	return getDefaultErrorPage(statusCode);
}

std::string	HttpResponse::getDefaultErrorPage(int statusCode)
{
	std::string errorText = getStatusText(statusCode);
	
	std::ifstream file("errorPages/errorPage.html");
	if (!file.is_open())

        return "<html><body><h1>Error " + Util::toString(statusCode) + "</h1><p>" + errorText + "</p></body></html>";
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string errorPage = buffer.str();

	std::map<std::string, std::string> replacements;
	replacements["{{statusCode}}"] = Util::toString(statusCode);
    replacements["{{errorText}}"] = errorText;

	std::map<std::string, std::string>::iterator it;
	for (it = replacements.begin(); it != replacements.end(); ++it) {
        size_t pos = 0;
        while ((pos = errorPage.find(it->first, pos)) != std::string::npos) {
            errorPage.replace(pos, it->first.length(), it->second);
            pos += it->second.length();
        }
    }
    return errorPage;
}

std::string HttpResponse::serialize()
{
	std::ostringstream  	oss;
	oss << "HTTP/1.1 " << status << " " << getStatusText(status) << "\r\n";
	
	setHeader("Server", WEBS_APP_NAME, true);
	setHeader("Content-Length", Util::toString(body.size() ) , true);

	for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		oss << Util::trim(it->first) << ": " << it->second << "\r\n";
	// single set of \r\n since the header already sent the first set
	oss << "\r\n" << body ;

	std::string color = (status >= 400) ? LC_RES_NOK_LOG : LC_RES_OK_LOG;
	Logger::log(color, "[RESPOND] %d - %s " , status, getStatusText(status).c_str() );

	if (WEBS_DEBUG_RESPONSE)
	{
		std::cout << LC_DEBUG << "===================================" << std::endl
		<< "response, size = " << oss.str().length() << std::endl
		<< "===================================" << LC_RESET << std::endl
		<< oss.str() << std::endl
		<< "===================================" << std::endl;
	}

	return oss.str();
	
}

bool HttpResponse::response(int socket_id)
{
	std::string 			wholeResponse = serialize();
	size_t					bytesSent = 0;

	while( bytesSent < wholeResponse.length())
	{
 		int sent = write(socket_id , wholeResponse.c_str() + bytesSent, wholeResponse.length() - bytesSent);
		if(sent <= 0)
			return false;
		bytesSent += sent;
	}
	
	// std::string testErrorPage = getDefaultErrorPage(status);
	return (true);
}

std::string HttpResponse::getMimeType(const std::string & extension)
{
	std::map<std::string, std::string> mimeTypes;
	mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/x-icon";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".txt"] = "text/plain";
	std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return it->second;
	return "application/octet-stream";
}

bool	HttpResponse::getStaticFile(std::string const &filePath )
{
	Logger::log(LC_YELLOW, " in getStaticFile() , filePath = " , filePath.c_str());

	std::ifstream file(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{		
		struct stat fileStat;

		Logger::log(LC_DEBUG, " getStaicFile ==> filePath = %s" , filePath.c_str());
		if (stat(filePath.c_str(), &fileStat) != 0)
		{
			if (errno == ENOENT)
				throw RequestException(404, "File not found");
			else if (errno == EACCES)
				throw RequestException(403, "Forbidden");
			else
				throw RequestException(405, "Method not allowed");
		}
		return false;
	}
	setStatus(200);

	size_t dotPos = filePath.find_last_of(".");
	std::string extension;
	if (dotPos != std::string::npos)
		extension = filePath.substr(dotPos);
	else
		extension = "";
	
	setHeader("Content-Type", getMimeType(extension), true);
	
	std::stringstream buffer;
	buffer << file.rdbuf();
	setBody(buffer.str());
	return true; 
}



void HttpResponse::debug() const
 {
		std::cout << "========================\n HttpResponse object:\n========================" << std::endl;
        std::cout << " - Status: \t" << std::endl;
        std::cout << " - Body Length: \t" << body.length() << std::endl;
        std::cout << " - Header: \t" << std::endl;
        for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            std::cout << "\t - " << it->first << " -> " << it->second << std::endl;
        }
		std::cout << std::endl;
}

bool HttpResponse::generateDirectoryListing(const std::string& path)
{
	DIR* dir = opendir(path.c_str());
	if (dir == NULL)
	{
		Logger::log(LC_DEBUG, " Getting Here???");
		std::cerr << "Error: " << strerror(errno) << std::endl;
		setStatus(403);
		return false;
	}

	std::ostringstream html;
	html << "<!DOCTYPE html>" << std::endl;
	// <link href='css/style.css' rel='stylesheet'>
	html << "<html><head><title>Directory Listing</title></head><body>" << std::endl;
	html << "<h1>Directory Listing for " << path << "</h1>" << std::endl;
	html << "<ul>" << std::endl;

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name != "." && name != "..")
		{
			html << "<li><a href=\"" << name << "\">" << name << "</a></li>" << std::endl;
		}
	}

	html << "</ul>" << std::endl;
	html << "</body></html>" << std::endl;

	closedir(dir);

	setStatus(200);
	setBody(html.str());
	return true;
}
