#include "DirectoryListing.hpp"

std::string DirectoryListing::generateListing(const std::string& path, const std::string& uri) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return "";
    }

    std::stringstream html;
    html << "<!DOCTYPE html>\n"
         << "<html>\n<head>\n"
         << "<title>Index of " << uri << "</title>\n"
         << "<style>\n"
         << "body { font-family: 'Inter', sans-serif; background: black; color: white; }\n"
         << "table { width: 100%; border-collapse: collapse; }\n"
         << "th, td { padding: 8px; text-align: left; border-bottom: 1px solid #444; }\n"
         << "a { color: #fff; text-decoration: none; }\n"
         << "a:hover { color: #aaa; }\n"
         << ".size { text-align: right; }\n"
         << "</style>\n"
         << "</head>\n<body>\n"
         << "<h1>Index of " << uri << "</h1>\n"
         << "<table>\n"
         << "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

    // Add parent directory link if not at root
    if (uri != "/") {
        html << "<tr><td><a href=\"..\">..</a></td><td>-</td><td>-</td></tr>\n";
    }

    std::vector<dirent*> entries;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::string(entry->d_name) != "." && 
            std::string(entry->d_name) != "..") {
            entries.push_back(entry);
        }
    }

    // Sort entries (directories first, then files)
    std::sort(entries.begin(), entries.end(), 
        [&path](const dirent* a, const dirent* b) {
            struct stat stat_a, stat_b;
            std::string full_path_a = path + "/" + a->d_name;
            std::string full_path_b = path + "/" + b->d_name;
            stat(full_path_a.c_str(), &stat_a);
            stat(full_path_b.c_str(), &stat_b);

            bool is_dir_a = S_ISDIR(stat_a.st_mode);
            bool is_dir_b = S_ISDIR(stat_b.st_mode);

            if (is_dir_a != is_dir_b)
                return is_dir_a > is_dir_b;
            return strcasecmp(a->d_name, b->d_name) < 0;
    });

    for (const auto& entry : entries) {
        std::string full_path = path + "/" + entry->d_name;
        struct stat st;
        if (stat(full_path.c_str(), &st) == 0) {
            std::string name = entry->d_name;
            if (S_ISDIR(st.st_mode))
                name += "/";

            html << "<tr>"
                 << "<td><a href=\"" << name << "\">" << name << "</a></td>"
                 << "<td>" << formatLastModified(st.st_mtime) << "</td>"
                 << "<td class=\"size\">" << (S_ISDIR(st.st_mode) ? "-" : formatFileSize(st.st_size)) << "</td>"
                 << "</tr>\n";
        }
    }

    html << "</table>\n</body>\n</html>";
    closedir(dir);
    return html.str();
}

std::string DirectoryListing::formatFileSize(off_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double filesize = static_cast<double>(size);

    while (filesize >= 1024 && unit < 3) {
        filesize /= 1024;
        unit++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(unit > 0 ? 1 : 0) << filesize << " " << units[unit];
    return ss.str();
}

std::string DirectoryListing::formatLastModified(time_t time) {
    char buffer[128];
    struct tm* timeinfo = localtime(&time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
    return std::string(buffer);
}

// Example usage in your request handler
if (isDirectory(path) && routeConfig.getAutoindex()) {
    std::string listing = DirectoryListing::generateListing(path, request.getUri());
    if (!listing.empty()) {
        response.setBody(listing);
        response.setHeader("Content-Type", "text/html");
        return;
    }
}