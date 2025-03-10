#include "RouteConfig.hpp"

RouteConfig::RouteConfig() : autoindex(false), clientMaxBodySize(1024 * 1024 * 8) {}

const std::string& RouteConfig::getPath() const { return path; }
const std::string& RouteConfig::getRoot() const { return root; }
const std::string& RouteConfig::getIndex() const { return index; }
const std::vector<std::string>& RouteConfig::getMethods() const { return allow_methods; }
// const std::string& RouteConfig::getCgi_path() const { return cgi_path; }
// const std::vector<std::string>& RouteConfig::getCgi_ext() const { return cgi_ext; }
const std::map<std::string, std::string>& RouteConfig::getCGIs() const { return CGIs; }
bool RouteConfig::getAutoindex() const { return autoindex; }
size_t RouteConfig::getClientMaxBodySize() const { return clientMaxBodySize; }

void RouteConfig::setPath(const std::string& path) { this->path = path; }
void RouteConfig::setRoot(const std::string& root) { this->root = root; }
void RouteConfig::setIndex(const std::string& index) { this->index = index; }
void RouteConfig::setMethods(const std::vector<std::string>& methods) { this->allow_methods = methods; }
// void RouteConfig::setCgi_path(const std::string& cgi_path) { this->cgi_path = cgi_path; }
// void RouteConfig::setCgi_ext(const std::vector<std::string>& cgi_ext) { this->cgi_ext = cgi_ext; }
// void RouteConfig::setCGIs(const std::map<std::string, std::string>& CGIs) { this->CGIs = CGIs; }
void RouteConfig::setCGIs(const std::string& ext, const std::string& path) { CGIs[ext] = path; }
void RouteConfig::setAutoindex(bool autoIndex) { this->autoindex = autoIndex; }
void RouteConfig::setClientMaxBodySize(size_t clientMaxBodySize) { this->clientMaxBodySize = clientMaxBodySize; }

void RouteConfig::debug() const
{
	std::cout << "\n====================\n Route Configuration\n====================" << std::endl;
	std::cout << "- Path: " << path << std::endl;
	std::cout << "- Root: " << root << std::endl;
	std::cout << "- Index: " << index << std::endl;
	
	std::cout << "- Allowed Methods: ";
	for (std::vector<std::string>::const_iterator it = allow_methods.begin(); 
		it != allow_methods.end(); ++it) {
		std::cout << *it;
		if (it + 1 != allow_methods.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
	
	std::cout << "- CGI Configurations:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = CGIs.begin(); 
		it != CGIs.end(); ++it) {
		std::cout << "  Extension: " << it->first << " -> Interpreter: " << it->second << std::endl;
	}
	
	std::cout << " - Autoindex: " << (autoindex ? "on" : "off") << std::endl;
	std::cout << " - Client Max Body Size: " << clientMaxBodySize << std::endl;
	std::cout << "====================\n" << std::endl;
}
