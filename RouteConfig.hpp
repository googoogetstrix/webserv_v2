#ifndef ROUTECONFIG_HPP
# define ROUTECONFIG_HPP

# include <iostream>
# include <map>
# include <string>
# include <vector>

class RouteConfig
{
	private:
		std::string path;
		std::string root;
		std::string index;
		std::vector<std::string> allow_methods;
		std::map<std::string, std::string> CGIs;
		bool autoindex;
		size_t clientMaxBodySize;

	public:
		RouteConfig();

		const std::string& getPath() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		const std::vector<std::string>& getMethods() const;
		const std::map<std::string, std::string>& getCGIs() const;
		bool getAutoindex() const;
		size_t getClientMaxBodySize() const;

		void setPath(const std::string& path);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setMethods(const std::vector<std::string>& methods);
		void setCGIs(const std::string& ext, const std::string& path);
		void setAutoindex(bool autoIndex);
		void setClientMaxBodySize(size_t clientMaxBodySize);

		void print() const
		{
			std::cout << "\n=== Route Configuration ===" << std::endl;
			std::cout << "Path: " << path << std::endl;
			std::cout << "Root: " << root << std::endl;
			std::cout << "Index: " << index << std::endl;
			
			std::cout << "Allowed Methods: ";
			for (std::vector<std::string>::const_iterator it = allow_methods.begin(); 
				it != allow_methods.end(); ++it) {
				std::cout << *it;
				if (it + 1 != allow_methods.end())
					std::cout << ", ";
			}
			std::cout << std::endl;
			
			std::cout << "CGI Configurations:" << std::endl;
			for (std::map<std::string, std::string>::const_iterator it = CGIs.begin(); 
				it != CGIs.end(); ++it) {
				std::cout << "  Extension: " << it->first << " -> Interpreter: " << it->second << std::endl;
			}
			
			std::cout << "Autoindex: " << (autoindex ? "on" : "off") << std::endl;
			std::cout << "Client Max Body Size: " << clientMaxBodySize << std::endl;
			std::cout << "==========================\n" << std::endl;
		}
};

#endif
