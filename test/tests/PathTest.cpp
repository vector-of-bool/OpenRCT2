#include <openrct2/util/path.hpp>

#include <gtest/gtest.h>
#include "AssertHelpers.hpp"

using TCase = std::tuple<std::string, std::string>;

class PathTest : public testing::TestWithParam<TCase>
{
};

using OpenRCT2::path;

TEST_F(PathTest, Normalization)
{
    for (auto & str : { "/foo/bar/", "/foo//bar", "/foo////bar", "/foo/bar//" })
    {
        ASSERT_EQ(path{ str }, "/foo/bar");
    }
}

TEST_F(PathTest, ParentDir)
{
    path p = "/foo/bar";
    ASSERT_EQ(p.parent_path(), "/foo");
    ASSERT_EQ(p.parent_path().parent_path(), "/");
    ASSERT_EQ(p.parent_path().parent_path().parent_path(), "/");
    p = "/foo/bar/.";
    ASSERT_EQ(p.parent_path(), "/foo/bar");
}

TEST_F(PathTest, Filename)
{
    path p = "/foo/bar";
    ASSERT_EQ(p.filename(), "bar");
    p = "/meow/";
    ASSERT_EQ(p.filename(), "meow");
    p = "/foo/.";
    ASSERT_EQ(p.filename(), ".");
}

TEST_F(PathTest, JoinPath)
{
    path a = "/foo";
    path b = "bar";
    ASSERT_EQ(a / b, "/foo/bar");
    ASSERT_EQ(a / b / b / b, "/foo/bar/bar/bar");
}