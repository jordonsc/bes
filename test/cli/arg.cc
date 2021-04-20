#include <bes/cli.h>
#include <gtest/gtest.h>

using b_t = std::tuple<std::string, std::string>;

class CliBoolTest : public testing::TestWithParam<b_t>
{
   public:
    CliBoolTest() {}

   protected:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(BooleanWords, CliBoolTest,
                         testing::Values(b_t("true", "false"), b_t("tRue", "fAlse"), b_t("True", "False"),
                                         b_t("yes", "no"), b_t("YES", "NO"), b_t("1", "0")));

class CliValueTest : public testing::TestWithParam<std::string>
{
   public:
    CliValueTest() {}

   protected:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(ValueArgs, CliValueTest, testing::Values("-b=20", "-cb=20", "--bbb=20", "--bbb=20"));

TEST(CliTest, Arg)
{
    using bes::cli::Arg;
    using bes::cli::ValueType;

    Arg arg('a', "aaa", "23");
    EXPECT_EQ(23, arg.as<int>());
    EXPECT_EQ("23", arg.as<std::string>());
    EXPECT_THROW(arg.as<bool>(), bes::cli::ValueErrorException);

    EXPECT_THROW(Arg('a', "aaa", "boo").as<int>(), bes::cli::ValueErrorException);
    EXPECT_THROW(Arg('a', "aaa").as<int>(), bes::cli::ValueErrorException);
    EXPECT_EQ(1, Arg('a', "aaa", "1.6").as<int>());
    EXPECT_EQ(1.6f, Arg('a', "aaa", "1.60").as<float>());
    EXPECT_EQ(1.6f, Arg('a', "aaa", " 1.6 ").as<float>());
    EXPECT_EQ(1.0f, Arg('a', "aaa", "1").as<float>());
    EXPECT_THROW(Arg('a', "aaa").as<float>(), bes::cli::ValueErrorException);
}

TEST(CliTest, ArgValue)
{
    using bes::cli::Arg;
    using bes::cli::ValueType;

    EXPECT_THROW(Arg('a', "aaa", "boo", ValueType::NONE), bes::cli::MalformedArgumentException);
    EXPECT_NO_THROW(Arg('a', "aaa", "boo", ValueType::OPTIONAL));
    EXPECT_NO_THROW(Arg('a', "aaa", "boo", ValueType::REQUIRED));
    EXPECT_NO_THROW(Arg('a', "aaa", "", ValueType::NONE));
    EXPECT_NO_THROW(Arg('a', "aaa", ValueType::OPTIONAL));
    EXPECT_NO_THROW(Arg('a', "aaa", ValueType::REQUIRED));
    EXPECT_NO_THROW(Arg('a', "aaa", ValueType::MANDATORY));
}

TEST_P(CliBoolTest, ArgBool)
{
    using bes::cli::Arg;
    auto [t_val, f_val] = GetParam();
    EXPECT_TRUE(Arg('t', "true_arg", t_val).as<bool>());
    EXPECT_FALSE(Arg('f', "false_arg", f_val).as<bool>());
    EXPECT_THROW(Arg('x', "error_arg", " false").as<bool>(), bes::cli::ValueErrorException);
}

TEST(CliTest, ParserTest)
{
    using bes::cli::Arg;
    bes::cli::Parser p;

    p.addArgument(Arg(0, "aaa")).addArgument('b', "bbb", "16");
    p << Arg('c', "ccc") << Arg('C', "ddd");
    EXPECT_EQ(4, p.argCount());

    EXPECT_THROW(p << Arg('b', "eee"), bes::cli::ArgumentConflictException);
    EXPECT_THROW(p << Arg('f', "ccc"), bes::cli::ArgumentConflictException);
    EXPECT_THROW(p << Arg('g', ""), bes::cli::MalformedArgumentException);

    EXPECT_EQ(4, p.argCount());
    EXPECT_EQ(16, p["bbb"].as<int>());
    EXPECT_EQ(0, p["bbb"].count());
    EXPECT_FALSE(p["bbb"].present());

    EXPECT_THROW(p["xxx"], bes::cli::NoSuchArgumentException);

    int argc = 3;
    char* argv[3] = {(char*)"test", (char*)"-ccCc", (char*)"--aaa"};

    p.parse(argc, argv);

    EXPECT_TRUE(p["aaa"].present());
    EXPECT_FALSE(p["bbb"].present());
    EXPECT_TRUE(p["ccc"].present());
    EXPECT_TRUE(p["ddd"].present());

    EXPECT_EQ(1, p["aaa"].count());
    EXPECT_EQ(0, p["bbb"].count());
    EXPECT_EQ(3, p["ccc"].count());
    EXPECT_EQ(1, p["ddd"].count());
}

TEST(CliTest, ParserMalformedTest)
{
    using bes::cli::Arg;
    bes::cli::Parser p;

    p << Arg(0, "aaa") << Arg('b', "bbb", "16") << Arg('c', "ccc") << Arg('d', "ddd");
    EXPECT_EQ(4, p.argCount());

    int argc = 2;
    char* argvA[2] = {(char*)"test", (char*)"--sd34 fd"};
    EXPECT_THROW(p.parse(argc, argvA), bes::cli::NoSuchArgumentException);

    char* argvB[2] = {(char*)"test", (char*)"-B"};
    EXPECT_THROW(p.parse(argc, argvB), bes::cli::NoSuchArgumentException);
}

TEST_P(CliValueTest, SetValueTest)
{
    using bes::cli::Arg;
    bes::cli::Parser p;

    p << Arg(Arg(0, "aaa")) << Arg('b', "bbb", "16") << Arg('c', "ccc") << Arg('d', "ddd");
    EXPECT_EQ(4, p.argCount());

    char param[40];
    strcpy(param, GetParam().c_str());

    int argc = 2;
    char* argv[2] = {(char*)"test", param};
    EXPECT_NO_THROW(p.parse(argc, argv));

    EXPECT_FALSE(p["aaa"].present());
    EXPECT_TRUE(p["bbb"].present());
    EXPECT_FALSE(p["ddd"].present());

    EXPECT_EQ(1, p["bbb"].count());
    EXPECT_EQ(20, p["bbb"].as<int>());
}

TEST(CliTest, ParserMandatoryTest)
{
    using bes::cli::Arg;
    using bes::cli::ValueType;
    bes::cli::Parser p;

    p << Arg(0, "aaa", ValueType::MANDATORY) << Arg('b', "bbb", "16") << Arg('c', "ccc") << Arg('d', "ddd");
    EXPECT_EQ(4, p.argCount());

    int argc = 2;
    char* argvA[2] = {(char*)"test", (char*)"-b"};
    char* argvB[2] = {(char*)"test", (char*)"--aaa=foo"};
    EXPECT_THROW(p.parse(argc, argvA), bes::cli::NoValueException);
    EXPECT_NO_THROW(p.parse(argc, argvB));
}
