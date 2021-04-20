#include <bes/app.h>
#include <gtest/gtest.h>

TEST(ServiceTest, Sidecar)
{
    using bes::net::Address;

    // Example 1: includes a default namespace
    bes::Config cfg_a;
    cfg_a.loadString(R"(---
discovery:
    config:
        port: 60001
        tld: "intranet"
        ns: "backend"
    service:
        app1.frontend: { host: "myapp", port: 20001 }
        app1.backend: { host: "myapp", port: 20002 }
        app2.backend: { host: "app2.dev" }
        app3.backend: { port: 5555 }
        app4.backend: { tld: ~ }
        app5: { host: "bad-specification" }
)");

    ASSERT_EQ("backend", cfg_a.get<std::string>("discovery", "config", "ns"));
    ASSERT_EQ(60001, cfg_a.get<uint16_t>("discovery", "config", "port"));
    ASSERT_EQ("myapp", cfg_a.get<std::string>("discovery", "service", "app1.frontend", "host"));
    ASSERT_EQ(20001, cfg_a.get<uint16_t>("discovery", "service", "app1.frontend", "port"));

    bes::app::discovery::Sidecar sidecar_a(cfg_a);
    ASSERT_TRUE(sidecar_a.ready());

    EXPECT_EQ("myapp:20002", sidecar_a.getServiceAddr("app1").addrFull());
    EXPECT_EQ("myapp:20001", sidecar_a.getServiceAddr("app1", "frontend").addrFull());
    EXPECT_EQ("app2.dev:60001", sidecar_a.getServiceAddr("app2").addrFull());
    EXPECT_EQ("app3.backend.intranet:5555", sidecar_a.getServiceAddr("app3").addrFull());
    EXPECT_EQ("app4.backend:60001", sidecar_a.getServiceAddr("app4").addrFull());
    EXPECT_EQ("app5.backend.intranet:60001", sidecar_a.getServiceAddr("app5").addrFull());

    // Example 2: no default namespace
    bes::Config cfg_b;
    cfg_b.loadString(R"(---
discovery:
    config:
        port: 60001
        tld: "intranet"
        ns: ~
    service:
        myapp: { host: "core.dev" }
        myapp.backend: { host: "core.backend" }
)");

    ASSERT_THROW(cfg_b.get<std::string>("discovery", "config", "ns"), bes::NullException);
    ASSERT_EQ(60001, cfg_b.get<uint16_t>("discovery", "config", "port"));
    ASSERT_EQ("core.dev", cfg_b.get<std::string>("discovery", "service", "myapp", "host"));

    bes::app::discovery::Sidecar sidecar_b(cfg_b);
    ASSERT_TRUE(sidecar_b.ready());

    EXPECT_EQ("core.dev:60001", sidecar_b.getServiceAddr("myapp").addrFull());
    EXPECT_EQ("myapp.frontend.intranet:60001", sidecar_b.getServiceAddr("myapp", "frontend").addrFull());
    EXPECT_EQ("core.backend:60001", sidecar_b.getServiceAddr("myapp", "backend").addrFull());

    // Example 3: no TLD or namespace
    bes::Config cfg_c;
    cfg_c.loadString(R"(---
discovery:
    config:
        port: 50001
    service:
        myapp: { host: "core.dev", port: 5555 }
        myapp.backend: { port: 12345 }
)");

    ASSERT_EQ(50001, cfg_c.get<uint16_t>("discovery", "config", "port"));

    bes::app::discovery::Sidecar sidecar_c(cfg_c);
    ASSERT_TRUE(sidecar_c.ready());

    EXPECT_EQ("some-app:50001", sidecar_c.getServiceAddr("some-app").addrFull());
    EXPECT_EQ("core.dev:5555", sidecar_c.getServiceAddr("myapp").addrFull());
    EXPECT_EQ("myapp.frontend:50001", sidecar_c.getServiceAddr("myapp", "frontend").addrFull());
    EXPECT_EQ("myapp.backend:12345", sidecar_c.getServiceAddr("myapp", "backend").addrFull());
}
