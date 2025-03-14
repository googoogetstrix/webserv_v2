/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:56:59 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/14 17:32:00 by bworrawa         ###   ########.fr       */
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
//	Logger::log(LC_RED, "HttpResponse instance is being destroyed");
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
		case 411:
			return "Length required";	
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
	
	std::ifstream file("errors/errorPage.html");
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

bool HttpResponse::generateDirectoryListing(const HttpRequest& request, const std::string& path)
{
	DIR* dir = opendir(path.c_str());
	if (dir == NULL)
	{
		Logger::log(LC_DEBUG, " Getting Here???");
		std::cerr << "Error: " << strerror(errno) << std::endl;
		setStatus(403);
		return false;
	}
	std::string relativePath = request.getPath();
	if (relativePath.empty() || relativePath[relativePath.size() - 1] != '/')
    {
        relativePath += '/';
    }

	std::ostringstream html;
	html << "<!DOCTYPE html>" << std::endl;
	// <link href='css/style.css' rel='stylesheet'>
	html << "<html><head><title>Directory Listing</title></head><body>" << std::endl;
	html << "<h1>Directory Listing for " << relativePath << "</h1>" << std::endl;
	html << "<ul>" << std::endl;

	html << "<li><a href=\"..\">.. (UP)</a></li>" << std::endl;
	
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name != "." && name != "..")
        {
            std::string fullPath = path + "/" + name;
            struct stat s;
            if (stat(fullPath.c_str(), &s) == 0)
            {
                if (s.st_mode & S_IFDIR)
                {
                    html << "<li><a href=\"" << relativePath << name << "/\">&#128193; " << name << "</a></li>" << std::endl;
                }
                else
                {
                    html << "<li><a href=\"" << relativePath << name << "\">&#128196; " << name << "</a></li>" << std::endl;
                }
            }
        }
    }

	html << "</ul>" << std::endl;
	html << "</body></html>" << std::endl;

	closedir(dir);

	setStatus(200);
	setBody(html.str());
	return true;
}

void HttpResponse::processPythonCGI(std::string command, std::string scriptFile, HttpRequest request, ServerConfig server, RouteConfig route, std::vector<char> &rawBytes)
{

	(void)server;
	(void)route;
	// const char *scriptPath = "processPlayer.py";

	std::cout << " ****  scriptFile = _" << scriptFile << "_" << std::endl; 
	// request.debug();
	
	char *const argv[] = {
		const_cast<char *>(command.c_str()), 
		const_cast<char *>(scriptFile.c_str())
		, NULL
	};
	std::string method = "REQUEST_METHOD=" + request.getMethod();
	std::string query = "QUERY_STRING=" + request.getRawQueryString();
	std::string contentType = "CONTENT_TYPE=" + request.getHeader("Content-Type");
	std::string contentLength = "CONTENT_LENGTH=" + Util::toString(request.getContentLength());
	std::string fileSize = "HTTP_FILESIZE=" + Util::toString(request.getContentLength());
	std::string scriptFilename = "SCRIPT_FILENAME=" + scriptFile;

	std::string path = "PATH=/usr/local/bin:/usr/bin:/bin";
	std::string serverSoftware = "SERVER_SOFTWARE=" + std::string(WEBS_APP_NAME);
	std::string envCGI = "GATEWAY_INTERFACE=CGI/1.1"; 
	std::string cookie = "HTTP_COOKIE=" + request.getHeader("Cookie");
	std::string accept = "HTTP_ACCEPT=" + request.getHeader("Accept");
	std::string status = "REDIRECT_STATUS=200";
	std::string uploadDir = "UPLOAD_DIR=/tmp";
	

	char * envp[] = {	
		const_cast<char *>(method.c_str()),
		const_cast<char *>(query.c_str()),
		const_cast<char *>(contentType.c_str()),
		const_cast<char *>(contentLength.c_str()),
		const_cast<char *>(uploadDir.c_str()),
		const_cast<char *>(fileSize.c_str()),
		const_cast<char *>(status.c_str()),
		const_cast<char *>(scriptFilename.c_str()),

		const_cast<char *>(path.c_str()), 
		const_cast<char *>(serverSoftware.c_str()), 
		const_cast<char *>(envCGI.c_str()), 
		const_cast<char *>(cookie.c_str()), 
		const_cast<char *>(accept.c_str()), 


		NULL
	};

	// Create pipes for stdin and stdout
	int pipe_stdin[2];
	int pipe_stdout[2];

	if (pipe(pipe_stdin) == -1 || pipe(pipe_stdout) == -1)
	{
		std::cerr << "Error creating pipes: " << strerror(errno) << std::endl;
		throw RequestException(500,"Internal Server Error");
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error forking process: " << strerror(errno) << std::endl;
		throw RequestException(500,"Internal Server Error");
	}
	if (pid == 0)
	{
		// sleep(10);
		// Child process
		if (dup2(pipe_stdin[0], STDIN_FILENO) == -1)
		{
			std::cerr << "Error redirecting stdin: " << strerror(errno) << std::endl;
			exit(errno) ;
		}
		close(pipe_stdin[0]);
		close(pipe_stdin[1]);

		// Redirect stdout
		// Logger::log(LC_RED, "RESTORE CGI pipeout HERE");
		if (dup2(pipe_stdout[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "Error redirecting stdout: " << strerror(errno) << std::endl;
			exit(errno) ;
		}
		// if (dup2(pipe_stdout[1], STDERR_FILENO) == -1)
		// {
		// 	std::cerr << "Error redirecting stderr: " << strerror(errno) << std::endl;
		// 	exit(errno) ;
		// }
		close(pipe_stdout[0]);
		close(pipe_stdout[1]);
		if (execve(argv[0], argv, envp) == -1)
		{
			std::cerr << "Error executing script: " << strerror(errno) << std::endl;
			exit(errno) ;
		}
	}
	else
	{
		// Parent process
		close(pipe_stdin[0]);
		close(pipe_stdout[1]);
	
		fcntl(pipe_stdout[0], F_SETFL, O_NONBLOCK);
		int  bytesWritten = write(pipe_stdin[1], rawBytes.data() , rawBytes.size());
		if(bytesWritten < 0)
		{
			close(pipe_stdin[1]);
            close(pipe_stdout[0]);
			throw RequestException(500,"Internal Server Error");
		}
		close(pipe_stdin[1]);

		// Read from the child's stdout
		char buffer[READ_BUFFER_SIZE];
		std::string  output = "";
		bool timed_out = false;
		int elapsed_time = 0;

		while (true)
        {
            ssize_t bytesRead = read(pipe_stdout[0], buffer, sizeof(buffer) - 1);
			// if (bytesRead > 0)
			// {
			// 	buffer[bytesRead] = '\0';
			// 	output += std::string(buffer);
			// }
			// else if (bytesRead == 0)
			// 	break;
			// else if (bytesRead < 0 && errno != EAGAIN)
			// {
			// 	std::cerr << "Error reading from pipe: " << strerror(errno) << std::endl;
			// 	close(pipe_stdout[0]);
			// 	throw RequestException(500,"Internal Server Error");
			// }
			while (bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				output += std::string(buffer);
				bytesRead = read(pipe_stdout[0], buffer, sizeof(buffer) - 1);
			}
			// else if (bytesRead < 0)
            // {
            //     if (errno == EAGAIN)
            //     {
            //         // No data available, try again later
            //         usleep(100000); // Sleep for 100 milliseconds
            //         elapsed_time += 100;
            //         if (elapsed_time >= CGI_TIMEOUT * 1000)
            //         {
            //             std::cerr << "Error: CGI script timed out" << std::endl;
            //             kill(pid, SIGKILL); // Terminate the child process
            //             timed_out = true;
            //             break;
            //         }
            //     }
            //     else
            //     {
            //         std::cerr << "Error reading from pipe: " << strerror(errno) << std::endl;
            //         close(pipe_stdout[0]);
            //         throw RequestException(500, "Internal Server Error");
            //     }
            // }

			// Wait for the child process to finish
			int status;
			pid_t result = waitpid(pid, &status, WNOHANG);
			if (result == 0)
			{
				usleep(100000);
				elapsed_time += 100;
				if (elapsed_time >= CGI_TIMEOUT * 1000)
				{
					std::cerr << "Error: CGI script timed out" << std::endl;
					kill(pid, SIGKILL);
					timed_out = true;
					break ;
				}
			}
			else if (result == -1)
			{
				std::cerr << "Error waiting for child process: " << strerror(errno) << std::endl;
				close(pipe_stdout[0]);
				throw RequestException(500,"Internal Server Error");
			}
			else
			{
				if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
				{
					setCGIResponse(output, output.length());
				}
				else
				{
					std::cerr << "Child process exited with error status: " << WEXITSTATUS(status) << std::endl;
					setStatus(500);
					setHeader("Content-Type", "text/html");
				}
				break ;
			}
		}
		close(pipe_stdout[0]);
		if (timed_out)
            throw RequestException(500,"Internal Server Error");
		
	}
}

bool	HttpResponse::isRepeatableHeader(std::string const &str)
{
	if(str.find("Set-Cookie") != std::string::npos)
		return true;
	if(str == "Cookie" || str == ("User-Agent"))
		return (true);
	return (false);
}


size_t	HttpResponse::setCGIResponse(std::string &output, size_t length)
{

	// char	buffer[5000];
	// memset(buffer,0,5000);

	int sepLength = 4;
	size_t splitPos = output.find("\r\n\r\n");
	if (splitPos == std::string::npos)
	{
		splitPos = output.find("\n\n");
		sepLength = 2; 
		if (splitPos == std::string::npos)
		{
			setBody("");
			setStatus(500);
			return (500);
		}
	}

	
	std::string buffer ;
	
	std::istringstream stream(output.substr(0, splitPos));
	
	std::string		headerName , headerValue ; 
    while (std::getline(stream, buffer))
	{
		std::istringstream token(buffer); 
		if( token >> headerName >> headerValue) 
		{
			headerName = headerName.substr(0 , headerName.length() - 1);
			std::cout << " _" << headerName << "_ , _" << headerValue << "_ " << std::endl;

			if(headerName == "Status")
			{
				setStatus(Util::toInt(headerValue));
			} else if(isRepeatableHeader(headerName))
				setHeader(headerName, headerValue, false);
			else 
				setHeader(headerName, headerValue, true);
			

		}
	}


	setBody( output.substr(splitPos + sepLength, length - (splitPos + sepLength)));
	return 200; 	

}


int 	HttpResponse::autoResponseHeader(HttpRequest &httpRequest)
{
		int	effected = 0;
		Logger::log(LC_YELLOW, " Inside autoResponseHeader ... ");
		for(std::map<std::string,std::string>::const_iterator it = httpRequest.getHeader().begin(); it != httpRequest.getHeader().end(); ++it)
		{
			if(Util::trim(it->first) == "Cookie")
			{
				setHeader("Set-Cookie" , it->second, false);
				Logger::log(LC_YELLOW, "\t - setting %s ", it->second.c_str());
				effected++;
			}

				
		}
		Logger::log(LC_YELLOW, "%d headers added", effected);
		return effected;

	
}

