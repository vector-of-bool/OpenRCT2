#ifndef UTIL_PATH_HPP_INCLUDED
#define UTIL_PATH_HPP_INCLUDED

#include <ostream>
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

    struct preprocessed_tag
    {
    };

    path(preprocessed_tag, string_type && str) : _str(std::move(str))
    {
    }

public:
    path() noexcept    = default;
    path(const path &) = default;
    path(path &&)      = default;
    path(string_type);
    path(const char * str) : path{ string_type{ str } }
    {
    }
    path(decltype(nullptr)) = delete;

    path & operator=(const path &) = default;
    path & operator=(path &&) = default;

    path parent_path() const;
    path filename() const;

    const char * data() const
    {
        return _str.data();
    }

    const string_type & string() const
    {
        return _str;
    }
};

inline bool operator==(const path & lhs, const path & rhs)
{
    return lhs.string() == rhs.string();
}

inline bool operator!=(const path & lhs, const path & rhs)
{
    return !(lhs == rhs);
}

inline std::ostream & operator<<(std::ostream & o, const path & p)
{
    o << '"' << p.string() << '"';
    return o;
}

inline path operator/(const path & lhs, const path & rhs)
{
    return path{ lhs.string() + "/" + rhs.string() };
}

} // namespace OpenRCT2

#endif // UTIL_PATH_HPP_INCLUDED