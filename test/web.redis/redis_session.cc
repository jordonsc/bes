#include <bes/web.redis.h>
#include <gtest/gtest.h>

#include <vector>

TEST(SessionTest, RedisSessionTest)
{
    bes::web::RedisSessionMgr mgr(bes::net::Address("127.0.0.1", 6379));
    mgr.setSessionTtl(60);

    auto session1 = mgr.createSession("S");
    auto session2 = mgr.createSession("S");
    EXPECT_NE(session1.sessionId(), session2.sessionId());

    EXPECT_EQ(0, session1.size());
    EXPECT_TRUE(session1.empty());
    session1.setValue("str", "Hello World");
    session1.setValue("int", int64_t(1213));
    session1.setValue("float", 12.345);
    session1.setValue("bool-f", false);
    session1.setValue("bool-t", true);
    EXPECT_EQ(5, session1.size());
    EXPECT_FALSE(session1.empty());

    EXPECT_EQ(0, session2.size());
    EXPECT_TRUE(session2.empty());

    mgr.persistSession(session1);

    auto session3 = mgr.getSession(session1.sessionId());

    EXPECT_EQ(session1.sessionId(), session3.sessionId());
    ASSERT_EQ(5, session3.size());
    EXPECT_FALSE(session3.empty());

    ASSERT_TRUE(session3.hasItem("float"));
    EXPECT_EQ(bes::web::SessionObject::ObjectType::DOUBLE, session3.getValue("float").data_type);
    EXPECT_EQ(12.345, session3.getDouble("float"));

    EXPECT_EQ("Hello World", session3.getString("str"));
    EXPECT_EQ(1213, session3.getInt("int"));
    EXPECT_FALSE(session3.getBool("bool-f"));
    EXPECT_TRUE(session3.getBool("bool-t"));
}
