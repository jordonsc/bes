#include <bes/net.h>
#include <gtest/gtest.h>

using namespace bes::net::http;

TEST(HttpTest, BasicTest)
{
    Request req;
    req.setOption(option::Timeout(5L));

    auto future = req.get("https://reqres.in/api/products/3");
    auto resp = future.get();

    EXPECT_EQ(200, resp.httpCode());
}
