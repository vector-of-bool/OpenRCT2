#include "path.h"
#include "path.hpp"

#include <cctype>

#include <openrct2/core/Math.hpp>

using OpenRCT2::path;

path path::parent_path() const
{
    const auto last = _str.rfind('/');
    if (last != _str.npos)
    {
        // No path separators. It's just a filename
        return ".";
    }
    if (last == 0)
    {
        // Path is in root
        return "/";
    }
    auto sub = _str.substr(0, last);
    return path{ sub };
}

path::path(path::string_type str)
{
#ifdef _WIN32
    // Convert Windows separators into POSIX ones, just for normalization
    for (auto & c : str)
    {
        if (c == '\\')
        {
            c = '/';
        }
    }
    // Process drive letter
    if (str.size() >= 2 && str[1] == ':')
    {
        // String has a drive letter
        // The first character must be a letter
        if (!std::isalpha(str[0]))
        {
            throw std::invalid_argument{ "Bad Windows path: " + str };
        }
        // Ensure drive letter is followed by a path separator
        if (str.size() == 2 || str[2] != '/')
        {
            str.insert(2, "/");
        }
    }
    // Check if path starts with a separator
    if (!str.empty() && str[0] == '/')
    {
        // Path starting with a seperator on Windows
        if (str.size() >= 2 && str[1] == '/')
        {
            // UNC path, leave it
        }
        else
        {
            // Insert a drive letter at the front of the path
            str.insert(0, "C:");
        }
    }
#endif
    string_type str_acc;
    str_acc.reserve(str.size());
    for (const char c : str)
    {
        if (c == '/')
        {
            if (str_acc.empty() || str_acc.back() != '/')
            {
                str_acc.push_back(c);
            }
            continue;
        }
        str_acc.push_back(c);
    }
}

// C wrappers
::orct2_path path_create()
{
    return new OpenRCT2::path;
}

void ::path_free(::orct2_path p)
{
    delete static_cast<OpenRCT2::path *>(p);
}

const char* ::path_c_str(::orct2_path p) {
    return static_cast<OpenRCT2::path*>(p)->data();
}

char * ::path_strdup(::orct2_path p)
{
    return ::_strdup(::path_c_str(p));
}