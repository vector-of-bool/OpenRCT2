#include "path.h"
#include "path.hpp"

#include <cctype>

#include <openrct2/core/Math.hpp>

using OpenRCT2::path;

path path::parent_path() const
{
    const auto last = _str.rfind('/');
    if (last == _str.npos)
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
    return path{ preprocessed_tag{}, std::move(sub) };
}

path path::filename() const
{
    const auto last = _str.rfind('/');
    if (last == _str.npos)
    {
        // Whole thing is a filename
        return *this;
    }
    else
    {
        return path{ preprocessed_tag{}, _str.substr(last + 1) };
    }
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
    // Strip trailing separators
    while (!str_acc.empty() && str_acc.back() == '/')
    {
        if (str_acc == "/")
        {
            // Root dir is an exception
            break;
        }
#ifdef _WIN32
        if (str_acc.size() == 3 && str_acc[1] == ':' && str_acc[2] == '/')
        {
            // Drive letter is an exception
            break;
        }
#endif
        // Remove the trailing separator
        str_acc.pop_back();
    }
    _str = str_acc;
}

// C wrappers
::orct2_path path_create()
{
    return new OpenRCT2::path;
}

void path_free(::orct2_path p)
{
    delete static_cast<OpenRCT2::path *>(p);
}

const char * path_c_str(::orct2_path p)
{
    return static_cast<OpenRCT2::path *>(p)->data();
}

char * path_strdup(::orct2_path p)
{
    return ::_strdup(::path_c_str(p));
}