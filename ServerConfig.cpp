#include "ServerConfig.hpp"
#include "Util.hpp"

ServerConfig::ServerConfig() : port(0), host("0.0.0.0"), clientMaxBodySize(1024 * 1024 * 8) {}

int ServerConfig::getPort() const { return port; }
const std::string& ServerConfig::getServerName() const { return serverName; }
const std::string& ServerConfig::getHost() const { return host; }
const std::string& ServerConfig::getRoot() const { return root; }
const std::string& ServerConfig::getIndex() const { return index; }
size_t ServerConfig::getClientMaxBodySize() const { return clientMaxBodySize; }
const std::map<int, std::string>& ServerConfig::getErrorPages() const { return errorPages; }
const std::map<std::string, RouteConfig>& ServerConfig::getRoutes() const { return routes; }

void ServerConfig::setPort(int port) { this->port = port; }
void ServerConfig::setServerName(const std::string& serverName) { this->serverName = serverName; }
void ServerConfig::setHost(const std::string& host) { this->host = host; }
void ServerConfig::setRoot(const std::string& root) { this->root = root; }
void ServerConfig::setIndex(const std::string& index) { this->index = index; }
void ServerConfig::setClientMaxBodySize(size_t clientMaxBodySize) { this->clientMaxBodySize = clientMaxBodySize; }
void ServerConfig::setErrorPages(const std::map<int, std::string>& errorPages) { this->errorPages = errorPages; }
void ServerConfig::setRoutes(const std::map<std::string, RouteConfig>& routes) { this->routes = routes; }

void ServerConfig::addRoute(const RouteConfig& routeConfig)
{
    routes[routeConfig.getPath()] = routeConfig;
}

void ServerConfig::addErrorPage(int errorCode, const std::string& path)
{
    errorPages[errorCode] = path;
}
std::string ServerConfig::getNick()
{
    return ( host + std::string(":") + Util::toString(port));
}

// RouteConfig 	*getRouteFromRequest(HttpRequest &httpRequest)
// {

//     (void) httpRequest;
//     std::string uri = httpRequest.getPath();
//     std::cout << "uri = " << uri << std::endl;
//     return NULL;
// }



RouteConfig     *ServerConfig::findRoute(std::string path)
{
	size_t		max = 0;
	RouteConfig *returnRoute = NULL;

	for(std::map<std::string,RouteConfig>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		// set default matching to first route found , WHICH requires to be /
		if (returnRoute == NULL)
			returnRoute = &(it->second); 

		if ( it->second.getPath() == path )
			return &(it->second);

		std::string loc = it->first;
		if ( loc[loc.size() - 1] != '/')
			loc += "/";
		
		if (path.find(loc) != std::string::npos)
		{
			size_t matchedLength = Util::charactersMatched(path, loc);
			
			if(matchedLength > max)
			{
				max = matchedLength;
				returnRoute = &(it->second);
			}
		}
	}
	return returnRoute;
}

bool	ServerConfig::resolveRoute(HttpRequest &httpRequest, RouteConfig &route, std::string &localPath , bool &allowDirectoryListing)
{
		localPath = httpRequest.getPath();
		std::string original = localPath;		
		std::string targetResource = Util::extractFileName(localPath, false);
		std::string filename = Util::extractFileName(localPath, true);

		if (targetResource.empty() && route.getIndex().empty() &&  !route.getAutoindex())
			throw RequestException(403, "Forbidden");

		// TODO - should we have ServerConfig level of this directive?
		allowDirectoryListing = false;
		allowDirectoryListing = route.getAutoindex();

		// original = (!Util::hasTrailingSlash(original) && original != "/")? "" : "***";
		// original = (!Util::hasTrailingSlash(original) && filename == "")? "***" : "";
		
		// std::cout << "original = " << original << std::endl;
		// ORIGINAL localPath.replace( 0, route.getPath().length(), "./" + route.getRoot() + original);	
		localPath.replace( 0, route.getPath().length(), "./" + route.getRoot() + "/");	


		// std::cout << " BEFORE APPENDING INDEX IS " << localPath << std::endl;
		if(filename.empty() && !route.getIndex().empty() && !route.getAutoindex())
		{
			if( !Util::hasTrailingSlash(localPath))
				localPath += "/";
			localPath += route.getIndex();
		}

		// std::cout << "localPath finally is << _" << localPath << "_" << std::endl;
		return (true);
}



void ServerConfig::debug() const
 {
		std::cout << "========================\n Server Configuration:\n========================" << std::endl;
        std::cout << " - Port: \t" << port << std::endl;
        std::cout << " - Host: \t" << host << std::endl;
        std::cout << " - Server Name:\t" << serverName << std::endl;
        std::cout << " - Root:\t" << root << std::endl;
        std::cout << " - Index:\t" << index << std::endl;

        std::cout << " - Error Pages:\t" << std::endl;
        std::map<int, std::string>::const_iterator it;
        for (it = errorPages.begin(); it != errorPages.end(); ++it) {
            std::cout << "    " << it->first << " -> " << it->second << std::endl;
        }
}
