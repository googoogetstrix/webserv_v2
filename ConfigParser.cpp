#include "ConfigParser.hpp"

std::string ConfigParser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v;");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> ConfigParser::split(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::string temp;
    std::istringstream stream(str);
    while (std::getline(stream, temp, delimiter))
        result.push_back(temp);
    return result;
}

int stringToInt(const std::string& str)
{
    std::stringstream ss(str);
    int result;
    ss >> result;
    return result;
}

RouteConfig ConfigParser::parseRouteConfig(std::ifstream& file, const std::string& locationPath)
{
    RouteConfig routeConfig;
    std::string line;
    std::vector<std::string> cgi_paths;

    routeConfig.setPath(locationPath);
    while (std::getline(file, line) && !line.empty())
    {
		line = trim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        if (line == "}")
            break ;
        else if (line.find("root") == 0 && line.length() > 5)
            routeConfig.setRoot(line.substr(5));
        else if (line.find("index") == 0 && line.length() > 6)
            routeConfig.setIndex(line.substr(6));
        else if (line.find("allow_methods") == 0 && line.length() > 14)
            routeConfig.setMethods(split(line.substr(14), ' '));
        else if (line.find("cgi_pass") == 0 && line.length() > 9)
        {
            size_t spacePos = line.find(' ', 9);
            if (spacePos != std::string::npos)
            {
                std::string ext = line.substr(9, spacePos - 9);
                std::string path = line.substr(spacePos + 1);
                routeConfig.setCGIs(ext, path);
            }
        } 
        else if (line.find("autoindex") == 0 && line.length() > 10)
            routeConfig.setAutoindex(line.substr(10) == "on");
        else if (line.find("client_max_body_size") == 0 && line.length() > 21)
            routeConfig.setClientMaxBodySize(stringToInt(line.substr(21)));
    }
    return routeConfig;
}

ServerConfig ConfigParser::parseConfig(std::ifstream& file)
{
    ServerConfig currentServerConfig;
    std::string line;
    std::string currentLocation;

    while (std::getline(file, line))
    {
		line = trim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        if (line == "}")
            return currentServerConfig;
        if (line.find("server") == 0)
            continue ;
        else if (line.find("listen") == 0 && line.length() > 7)
            currentServerConfig.setPort(stringToInt(line.substr(7)));
        else if (line.find("server_name") == 0 && line.length() > 12)
            currentServerConfig.setServerName(line.substr(12));
        else if (line.find("host") == 0 && line.length() > 5)
            currentServerConfig.setHost(line.substr(5));
        else if (line.find("root") == 0 && line.length() > 5)
            currentServerConfig.setRoot(line.substr(5));
        else if (line.find("index") == 0 && line.length() > 6)
            currentServerConfig.setIndex(line.substr(6));
        else if (line.find("error_page") == 0)
        {
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos && line.length() > spacePos + 5)
            {
                int errorCode = stringToInt(line.substr(spacePos + 1, 3));
                std::string errorPagePath = line.substr(spacePos + 5);
                currentServerConfig.addErrorPage(errorCode, errorPagePath);
            }
        }
        else if (line.find("location") == 0)
        {   
            std::string locationLine = line.substr(9);
            size_t bracePos = locationLine.find('{');
            if (bracePos != std::string::npos)
                currentLocation = trim(locationLine.substr(0, bracePos));
            else
                currentLocation = trim(locationLine);

            RouteConfig routeConfig = parseRouteConfig(file, currentLocation);
            currentServerConfig.addRoute(routeConfig);
        }
    }
    return currentServerConfig;
}

bool ConfigParser::isValidServerConfig(const ServerConfig& config)
{
    // return config.getPort() > 0 && !config.getHost().empty() && !config.getRoot().empty();
    return config.getPort() > 0 && !config.getHost().empty();
}

std::vector<ServerConfig> ConfigParser::parseAllConfigs(const std::string& configFilePath)
{
    std::vector<ServerConfig> serverConfigs;
    std::ifstream file(configFilePath.c_str());
    std::string line;

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open config file: " + configFilePath);
    }

    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        if (line.find("server") == 0)
        {
            ServerConfig serverConfig = parseConfig(file);
            if (!isValidServerConfig(serverConfig))
            {
                // throw std::runtime_error("Invalid server configuration");
                continue ;
            }
            serverConfigs.push_back(serverConfig);
        }
    }
    file.close();
    return serverConfigs;
}
