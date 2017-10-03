#ifndef UTIL_PATH_HPP_INCLUDED
#define UTIL_PATH_HPP_INCLUDED

#include <string>

#include "path.h"

struct orct2_path_t
{
};

namespace OpenRCT2
{

class path : public orct2_path_t
{
public:
    using string_type = std::string;

private:
    string_type _str;

public:
    path() noexcept    = default;
    path(const path &) = default;
    path(path &&)      = default;
    path(string_type);
    path(const char * str) : path{ string_type{ str } }
    {
    }

    path & operator=(const path &) = default;
    path & operator=(path &&) = default;

    path parent_path() const;

    const char * data() const
    {
        return _str.data();
    }
};

} // namespace OpenRCT2

#endif // UTIL_PATH_HPP_INCLUDED