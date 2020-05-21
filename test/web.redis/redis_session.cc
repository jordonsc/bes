#include <bes/web.redis.h>
#include <gtest/gtest.h>

#include <vector>

TEST(SessionTest, RedisSessionTest)
{
    bes::web::RedisSessionMgr mgr(bes::net::Address("127.0.0.1", 6379));
    mgr.SetSessionTtl(60);

    auto session1 = mgr.CreateSession();
    auto session2 = mgr.CreateSession();
    EXPECT_NE(session1.SessionId(), session2.SessionId());

    EXPECT_EQ(0, session1.Size());
    EXPECT_TRUE(session1.Empty());
    session1.SetValue("str", "Hello World");
    session1.SetValue("int", 1213);
    session1.SetValue("float", 12.345);
    session1.SetValue("bool-f", false);
    session1.SetValue("bool-t", true);
    EXPECT_EQ(5, session1.Size());
    EXPECT_FALSE(session1.Empty());

    EXPECT_EQ(0, session2.Size());
    EXPECT_TRUE(session2.Empty());

    mgr.PersistSession(session1);

    auto session3 = mgr.GetSession(session1.SessionId());

    EXPECT_EQ(session1.SessionId(), session3.SessionId());
    ASSERT_EQ(5, session3.Size());
    EXPECT_FALSE(session3.Empty());

    ASSERT_TRUE(session3.HasItem("float"));
    EXPECT_EQ(bes::web::SessionObject::ObjectType::DOUBLE, session3.GetValue("float").data_type);
    EXPECT_EQ(12.345, session3.GetDouble("float"));

    EXPECT_EQ("Hello World", session3.GetString("str"));
    EXPECT_EQ(1213, session3.GetInt("int"));
    EXPECT_FALSE(session3.GetBool("bool-f"));
    EXPECT_TRUE(session3.GetBool("bool-t"));
}
