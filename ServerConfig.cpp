#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : port(0), host("0.0.0.0"), clientMaxBodySize(1024 * 1024 * 8) {}

int ServerConfig::getPort() const { return port; }
const std::vector<std::string>& ServerConfig::getServerNames() const { return serverNames; }
const std::string& ServerConfig::getHost() const { return host; }
const std::string& ServerConfig::getRoot() const { return root; }
const std::string& ServerConfig::getIndex() const { return index; }
size_t ServerConfig::getClientMaxBodySize() const { return clientMaxBodySize; }
const std::map<int, std::string>& ServerConfig::getErrorPages() const { return errorPages; }
const std::vector<RouteConfig>& ServerConfig::getRoutes() const { return routes; }

void ServerConfig::setPort(int port) { this->port = port; }
void ServerConfig::setServerNames(const std::vector<std::string>& serverNames) { this->serverNames = serverNames; }
void ServerConfig::setHost(const std::string& host) { this->host = host; }
void ServerConfig::setRoot(const std::string& root) { this->root = root; }
void ServerConfig::setIndex(const std::string& index) { this->index = index; }
void ServerConfig::setClientMaxBodySize(size_t clientMaxBodySize) { this->clientMaxBodySize = clientMaxBodySize; }
void ServerConfig::setErrorPages(const std::map<int, std::string>& errorPages) { this->errorPages = errorPages; }
void ServerConfig::setRoutes(const std::vector<RouteConfig>& routes) { this->routes = routes; }

void ServerConfig::addRoute(const RouteConfig& routeConfig)
{
    routes.push_back(routeConfig);
}

void ServerConfig::addErrorPage(int errorCode, const std::string& path)
{
    errorPages[errorCode] = path;
}
