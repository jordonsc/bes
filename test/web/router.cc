#include <bes/web.h>
#include <gtest/gtest.h>

#include <vector>

constexpr auto routes = R"--EOF--(---
home:
  uri: /

about_us:
  uri: /about-us
  includes_query: true
  controller: info

page:
  uri: "/section/{ section: [A-Za-z0-9-_: ]{3,5} }?page={ id }"
  includes_query: true

)--EOF--";

TEST(WebTest, RouteGenTest)
{
    bes::web::MappedRouter router;

    router.loadRoutesFromString(routes);
    bes::web::Route cat("category", "/category/{ cat: [A-Z]{3} }/");
    router.addRoute(cat);

    auto const& map = router.routeMap();

    ASSERT_EQ(4, map.size());

    {
        // 'home'
        bes::web::Route route = map.at("home");
        EXPECT_EQ("home", route.name);
        EXPECT_EQ("home", route.controller);
        EXPECT_EQ(false, route.includes_query);
        ASSERT_EQ(1, route.parts.size());
        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[0].section_type);
        EXPECT_EQ("", route.parts[0].name);
        EXPECT_EQ("/", route.parts[0].value);
    }

    {
        // 'about_us'
        bes::web::Route route = map.at("about_us");
        EXPECT_EQ("about_us", route.name);
        EXPECT_EQ("info", route.controller);
        EXPECT_EQ(true, route.includes_query);
        ASSERT_EQ(1, route.parts.size());
        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[0].section_type);
        EXPECT_EQ("", route.parts[0].name);
        EXPECT_EQ("/about-us", route.parts[0].value);
    }

    {
        // 'category'
        bes::web::Route route = map.at("category");
        EXPECT_EQ("category", route.name);
        EXPECT_EQ("category", route.controller);
        EXPECT_EQ(false, route.includes_query);
        ASSERT_EQ(3, route.parts.size());

        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[0].section_type);
        EXPECT_EQ("", route.parts[0].name);
        EXPECT_EQ("/category/", route.parts[0].value);

        EXPECT_EQ(bes::web::RouteType::REGEX, route.parts[1].section_type);
        EXPECT_EQ("cat", route.parts[1].name);
        EXPECT_EQ("[A-Z]{3}", route.parts[1].value);

        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[2].section_type);
        EXPECT_EQ("", route.parts[2].name);
        EXPECT_EQ("/", route.parts[2].value);
    }

    {
        // 'page'
        bes::web::Route route = map.at("page");
        EXPECT_EQ("page", route.name);
        EXPECT_EQ("page", route.controller);
        EXPECT_EQ(true, route.includes_query);
        ASSERT_EQ(4, route.parts.size());

        // /section/
        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[0].section_type);
        EXPECT_EQ("", route.parts[0].name);
        EXPECT_EQ("/section/", route.parts[0].value);

        // { section: [A-Za-z0-9: ]{3,5} }
        EXPECT_EQ(bes::web::RouteType::REGEX, route.parts[1].section_type);
        EXPECT_EQ("section", route.parts[1].name);
        EXPECT_EQ("[A-Za-z0-9-_: ]{3,5}", route.parts[1].value);

        // ?page=
        EXPECT_EQ(bes::web::RouteType::LITERAL, route.parts[2].section_type);
        EXPECT_EQ("", route.parts[2].name);
        EXPECT_EQ("?page=", route.parts[2].value);

        // { id }
        EXPECT_EQ(bes::web::RouteType::REGEX, route.parts[3].section_type);
        EXPECT_EQ("id", route.parts[3].name);
        EXPECT_EQ(".+?", route.parts[3].value);
    }
}

TEST(WebTest, UriGenTest)
{
    bes::web::MappedRouter router;
    router.loadRoutesFromString(routes);

    EXPECT_EQ("/", router.getUri("home"));
    EXPECT_EQ("/about-us", router.getUri("about_us"));
    EXPECT_THROW(router.getUri("page"), bes::web::MissingArgumentException);
    EXPECT_EQ("/section/foo?page=bar", router.getUri("page", {{"section", "foo"}, {"id", "bar"}}));
}

TEST(WebTest, RouteMatchTest)
{
    bes::web::MappedRouter router;
    router.loadRoutesFromString(routes);

    ASSERT_EQ(router.size(), 3);

    router.addRoute(bes::web::Route("category", "/category/{ cat: [A-Z]{3} }/"));
    ASSERT_EQ(router.size(), 4);

    {
        auto [route, args] = router.findRoute("/about-us", "");
        EXPECT_EQ("about_us", route.name);
    }

    {
        auto [route, args] = router.findRoute("/");
        EXPECT_EQ("home", route.name);
    }

    {
        auto [route, args] = router.findRoute("/category/HDX/");
        EXPECT_EQ("category", route.name);
    }

    {
        auto [route, args] = router.findRoute("/section/foo_t", "page=123");
        EXPECT_EQ("page", route.name);
        ASSERT_EQ(2, args.size());
        EXPECT_EQ("foo_t", args["section"]);
        EXPECT_EQ("123", args["id"]);
    }

    EXPECT_THROW(router.findRoute("/section/this-is-not-valid", "page=xxx"), bes::web::NoMatchException);
    EXPECT_THROW(router.findRoute("/category/abc/"), bes::web::NoMatchException);
}