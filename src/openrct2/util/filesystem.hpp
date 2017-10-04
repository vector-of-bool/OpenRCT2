#ifndef UTIL_FILESYSTEM_HPP_INCLUDED
#define UTIL_FILESYSTEM_HPP_INCLUDED

#include "path.hpp"

#include <system_error>

namespace OpenRCT2
{

bool exists(const path & p);
bool is_file(const path & p);
bool is_directory(const path & p);
void rename(const path & src, const path & dst);
void rename(const path & src, const path & dst, std::error_code & ec) noexcept;
void remove(const path & p);
void remove(const path & p, std::error_code & ec) noexcept;

enum class copy_options {
    none = 0,
    skip_existing = 1 << 0,
    overwrite_existing = 1 << 1,
};

inline copy_options operator|(copy_options a, copy_options b) {
    return static_cast<copy_options>(static_cast<int>(a) | static_cast<int>(b));
}

inline copy_options operator&(copy_options a, copy_options b) {
    return static_cast<copy_options>(static_cast<int>(a) & static_cast<int>(b));
}

inline copy_options operator^(copy_options a, copy_options b) {
    return static_cast<copy_options>(static_cast<int>(a) ^ static_cast<int>(b));
}

void copy(const path & from, const path & to, copy_options, std::error_code & ec) noexcept;

inline void copy(const path & from, const path & to, std::error_code & ec) noexcept
{
    copy(from, to, copy_options::none, ec);
}

inline void copy(const path & from, const path & to, copy_options opts)
{
    std::error_code ec;
    copy(from, to, opts, ec);
    if (ec)
    {
        throw std::system_error{ ec, "copy()" };
    }
}

inline void copy(const path & from, const path & to)
{
    copy(from, to, copy_options::none);
}

} // namespace OpenRCT2

#endif // UTIL_FILESYSTEM_HPP_INCLUDED