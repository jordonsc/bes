#include "gtest/gtest.h"
#include "bes/templating.h"

using bes::templating::syntax::Expression;
using Clause = bes::templating::syntax::Expression::Clause;
using Operator = bes::templating::syntax::Expression::Operator;
using bes::templating::syntax::Symbol;
using SymbolType = bes::templating::syntax::Symbol::SymbolType;
using DataType = bes::templating::syntax::Symbol::DataType;

TEST(TemplatingTest, ExpressionValue)
{
    {
        Expression exp("bar1_");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("bar1_", exp.left.Value<std::string>());
        EXPECT_EQ(SymbolType::VARIABLE, exp.left.symbol_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("\"bar\"");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("bar", exp.left.Value<std::string>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::STRING, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("\"hello world\"");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("hello world", exp.left.Value<std::string>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::STRING, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("'c'");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ('c', exp.left.Value<char>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::CHAR, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("42");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ(42, exp.left.Value<int>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::INT, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("42.5");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ(42.5f, exp.left.Value<float>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::FLOAT, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp(" not 001042.05030  ");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ(1042.0503f, exp.left.Value<float>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::FLOAT, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_TRUE(exp.negated);
    }

    {
        Expression exp("false");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_FALSE(exp.left.Value<bool>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::BOOL, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("true");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_TRUE(exp.left.Value<bool>());
        EXPECT_EQ(SymbolType::LITERAL, exp.left.symbol_type);
        EXPECT_EQ(DataType::BOOL, exp.left.data_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("not foo");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ(SymbolType::VARIABLE, exp.left.symbol_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_TRUE(exp.negated);
    }

    {
        Expression exp("not foo.bar");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ("bar", exp.left.Value<std::string>(1));
        EXPECT_EQ(SymbolType::VARIABLE, exp.left.symbol_type);
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_TRUE(exp.negated);
    }
}

TEST(TemplatingTest, ExpressionFor)
{
    {
        Expression exp("for foo in bar");
        EXPECT_EQ(Clause::FOR, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ("bar", exp.right.Value<std::string>());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("for foo in [alpha, \"bravo\", 32, true]");
        EXPECT_EQ(Clause::FOR, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ(DataType::NONE, exp.right.data_type);
        EXPECT_EQ(SymbolType::ARRAY, exp.right.symbol_type);
        EXPECT_EQ(4, exp.right.items.size());
        EXPECT_FALSE(exp.negated);

        EXPECT_EQ(SymbolType::VARIABLE, exp.right.Value<Symbol>().symbol_type);
        EXPECT_EQ("alpha", exp.right.Value<Symbol>().Value<std::string>());

        EXPECT_EQ(DataType::STRING, exp.right.Value<Symbol>(1).data_type);
        EXPECT_EQ("bravo", exp.right.Value<Symbol>(1).Value<std::string>());

        EXPECT_EQ(DataType::INT, exp.right.Value<Symbol>(2).data_type);
        EXPECT_EQ(32, exp.right.Value<Symbol>(2).Value<int>());

        EXPECT_EQ(DataType::BOOL, exp.right.Value<Symbol>(3).data_type);
        EXPECT_TRUE(exp.right.Value<Symbol>(3).Value<bool>());
    }

    {
        Expression exp("for hello not in world");
        EXPECT_EQ(Clause::FOR, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("hello", exp.left.Value<std::string>());
        EXPECT_EQ("world", exp.right.Value<std::string>());
        EXPECT_TRUE(exp.negated);
    }

    {
        Expression exp("  for barry   not  in      world    ");
        EXPECT_EQ(Clause::FOR, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("barry", exp.left.Value<std::string>());
        EXPECT_EQ("world", exp.right.Value<std::string>());
        EXPECT_TRUE(exp.negated);
    }
}

TEST(TemplatingTest, ExpressionIf)
{
    {
        Expression exp("if foo not in bar");
        EXPECT_EQ(Clause::IF, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ("bar", exp.right.Value<std::string>());
        EXPECT_TRUE(exp.negated);
    }

    {
        Expression exp("if foo in bar");
        EXPECT_EQ(Clause::IF, exp.clause);
        EXPECT_EQ(Operator::IN, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ("bar", exp.right.Value<std::string>());
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("if not foo");
        EXPECT_EQ(Clause::IF, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("foo", exp.left.Value<std::string>());
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_TRUE(exp.negated);
    }

    {
        Expression exp("if bar");
        EXPECT_EQ(Clause::IF, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("bar", exp.left.Value<std::string>());
        EXPECT_EQ(0, exp.right.items.size());
        EXPECT_FALSE(exp.negated);
    }
}

class TemplatingTestMalformed : public testing::TestWithParam<std::string>
{
   public:
    TemplatingTestMalformed() {}

   protected:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(ValueArgs, TemplatingTestMalformed,
                         testing::Values("not if foo", "for i in foo bar", "not if foo", "not if foo", "for foo",
                                         "for not foo", "not not foo", "if foo not", "not foo.b-ar", "3fa",
                                         "foo.\"bar\"", "123.456.789", "'hello'"));

TEST_P(TemplatingTestMalformed, ExpressionMalformed)
{
    EXPECT_THROW(Expression a(GetParam()), bes::templating::MalformedExpressionException);
}


TEST(TemplatingTest, Filters)
{
    {
        Expression exp("hello|world");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("hello", exp.left.Value<std::string>());
        EXPECT_EQ(0, exp.right.items.size());
        ASSERT_EQ(1, exp.filters.size());
        EXPECT_EQ("world", exp.filters[0]);
        EXPECT_FALSE(exp.negated);
    }

    {
        Expression exp("hello |world | stuff | foo|bar");
        EXPECT_EQ(Clause::VALUE, exp.clause);
        EXPECT_EQ(Operator::NONE, exp.op);
        EXPECT_EQ("hello", exp.left.Value<std::string>());
        EXPECT_EQ(0, exp.right.items.size());
        ASSERT_EQ(4, exp.filters.size());
        EXPECT_EQ("world", exp.filters[0]);
        EXPECT_EQ("stuff", exp.filters[1]);
        EXPECT_EQ("foo", exp.filters[2]);
        EXPECT_EQ("bar", exp.filters[3]);
        EXPECT_FALSE(exp.negated);
    }
}