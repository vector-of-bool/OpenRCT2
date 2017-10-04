#include "filesystem.hpp"

#include <sys/stat.h>

#include <fstream>

bool OpenRCT2::exists(const path & p)
{
    return access(p.data(), F_OK) != -1;
}

bool OpenRCT2::is_file(const path & p)
{
    struct stat stat;
    const auto  result = ::stat(p.data(), &stat);
    if (result != 0 || !S_ISREG(stat.st_mode))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool OpenRCT2::is_directory(const path & p)
{
    struct stat stat;
    const auto  result = ::stat(p.data(), &stat);
    if (result != 0 || !S_ISDIR(stat.st_mode))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void OpenRCT2::rename(const path & src, const path & dst)
{
    std::error_code ec;
    rename(src, dst, ec);
    if (ec)
    {
        throw std::system_error{ ec, "rename()" };
    }
}

void OpenRCT2::rename(const path & src, const path & dst, std::error_code & ec) noexcept
{
    const auto retc = ::rename(src.data(), dst.data());
    if (retc)
    {
        ec = std::error_code{ errno, std::system_category() };
    }
}

void OpenRCT2::remove(const path & p)
{
    std::error_code ec;
    remove(p, ec);
    if (ec)
    {
        throw std::system_error{ ec, "remove(): " + p.string() };
    }
}

void OpenRCT2::remove(const path & p, std::error_code & ec) noexcept
{
    const auto retc = ::unlink(p.data());
    if (retc)
    {
        ec = std::error_code{ errno, std::system_category() };
    }
}

void OpenRCT2::copy(const path & src, const path & dst, copy_options opts, std::error_code & ec) noexcept
{
    if (exists(dst))
    {
        if ((opts & copy_options::overwrite_existing) != copy_options::none)
        {
            ec = make_error_code(std::errc::file_exists);
            return;
        }
        // We are told to overwrite the file. Okay.
    }
    std::ofstream ofile{ dst.string(), std::ios_base::binary };
    if (!ofile)
    {
        ec = std::error_code{ errno, std::system_category() };
        return;
    }
    std::ifstream infile{ src.string(), std::ios_base::binary };
    if (!infile)
    {
        ec = std::error_code{ errno, std::system_category() };
        return;
    }
    using in_iter = std::istreambuf_iterator<char>;
    std::copy(in_iter{ infile }, in_iter{}, std::ostreambuf_iterator<char>{ ofile });
}