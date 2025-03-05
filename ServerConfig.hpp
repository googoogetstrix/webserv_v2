#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include "RouteConfig.hpp"

class ServerConfig
{
	private:
		int port;
		std::string serverName;
		std::string host;
		std::string root;
		std::string index;
		size_t clientMaxBodySize;
		std::map<int, std::string> errorPages;
		std::map<std::string, RouteConfig> routes;

	public:
		ServerConfig();

		int getPort() const;
		const std::string& getServerName() const;
		const std::string& getHost() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		size_t getClientMaxBodySize() const;
		const std::map<int, std::string>& getErrorPages() const;
		const std::map<std::string, RouteConfig>& getRoutes() const;

		void setPort(int port);
		void setServerName(const std::string& serverName);
		void setHost(const std::string& host);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setClientMaxBodySize(size_t clientMaxBodySize);
		void setErrorPages(const std::map<int, std::string>& errorPages);
		void setRoutes(const std::map<std::string, RouteConfig>& routes);
		
		
		void addRoute(const RouteConfig& routeConfig);
		void addErrorPage(int errorCode, const std::string& path);

		void print() const {
        std::cout << "Server Configuration: " << std::endl;
        std::cout << "  Port: " << port << std::endl;
        std::cout << "  Host: " << host << std::endl;
        std::cout << "  Server Name: " << serverName << std::endl;
        std::cout << "  Root: " << root << std::endl;
        std::cout << "  Index: " << index << std::endl;

        std::cout << "  Error Pages:" << std::endl;
        std::map<int, std::string>::const_iterator it;
        for (it = errorPages.begin(); it != errorPages.end(); ++it) {
            std::cout << "    " << it->first << " -> " << it->second << std::endl;
        }

        // std::cout << "  Locations:" << std::endl;
        // for (size_t i = 0; i < routes.size(); ++i) {
        //     routes[i].print();
        // }
    }
};

#endif
