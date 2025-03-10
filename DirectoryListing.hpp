#ifndef DIRECTORYLISTING_HPP
#define DIRECTORYLISTING_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <ctime>

class DirectoryListing {
public:
    static std::string generateListing(const std::string& path, const std::string& uri);
private:
    static std::string formatFileSize(off_t size);
    static std::string formatLastModified(time_t time);
};

#endif