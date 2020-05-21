#include <bes/templating.h>
#include <gtest/gtest.h>

#include <utility>

struct Document
{
    Document(std::string title, std::string heading) : title(std::move(title)), heading(std::move(heading)) {}

    std::string title;
    std::string heading;
    std::string sub_heading;
    int article_id = 0;
    int owner_id = 0;
    std::vector<std::string> paragraphs;
};

template <>
class bes::templating::data::StandardShell<std::shared_ptr<Document>> : public bes::templating::data::ShellInterface
{
   public:
    explicit StandardShell(std::shared_ptr<Document> item) : item(std::move(item)) {}

    [[nodiscard]] inline std::shared_ptr<ShellInterface> ChildNode(std::string const& key) const override
    {
        if (key == "title") {
            return std::make_shared<StandardShell<std::string>>(item->title);
        } else if (key == "heading") {
            return std::make_shared<StandardShell<std::string>>(item->heading);
        } else if (key == "sub_heading") {
            return std::make_shared<StandardShell<std::string>>(item->sub_heading);
        } else if (key == "article_id") {
            return std::make_shared<StandardShell<int>>(item->article_id);
        } else if (key == "owner_id") {
            return std::make_shared<StandardShell<int>>(item->owner_id);
        } else if (key == "paragraphs") {
            return std::make_shared<StandardShell<std::vector<std::string>>>(item->paragraphs);
        } else {
            throw IndexErrorException(BES_TEMPLATING_NO_NODE + key);
        }
    }

    [[nodiscard]] size_t Count() const override
    {
        return item->paragraphs.size();
    }

    void Begin() override
    {
        it = item->paragraphs.begin();
    }

    std::shared_ptr<ShellInterface> Yield() override
    {
        return std::make_shared<StandardShell<std::string>>(*it++);
    }

   protected:
    std::shared_ptr<Document> item;
    std::vector<std::string>::iterator it;
};

TEST(TemplatingRenderTest, BasicRender)
{
    auto doc = std::make_shared<Document>("Sample App", "   Hello World   ");
    doc->paragraphs.emplace_back("Did I ever tell you the definition of insanity?");
    doc->paragraphs.emplace_back("We're all mad here, I'm mad, you're mad.");
    doc->paragraphs.emplace_back("How do you know I'm mad?");
    doc->paragraphs.emplace_back("You must be, or you wouldn't have come.");

    bes::templating::Engine engine;
    bes::templating::data::ContextBuilder ctx;

    std::string template_base = getenv("TEST_SRCDIR") + std::string("/bes/test/data");
    engine.search_path.AppendSearchPath(template_base);

    engine.LoadFile("sample", "hello-world.tmpl.html.twig");
    engine.LoadFile("base", "hello-world.base.html.twig");
    engine.LoadFile("snippet_ext", "snippet_ext.html.twig");
    engine.LoadFile("snippet_base", "snippet_base.html.twig");

    ctx.Set("doc", doc);

    {
        std::ifstream str(template_base + "/hello-world.out-1.html");
        std::stringstream buffer;
        buffer << str.rdbuf();
        EXPECT_EQ(buffer.str(), engine.Render("sample", ctx.GetContext()));
    }

    {
        doc->article_id = 48220;
        doc->sub_heading = "By Randy McRandleson";

        std::ifstream str(template_base + "/hello-world.out-2.html");
        std::stringstream buffer;
        buffer << str.rdbuf();
        EXPECT_EQ(buffer.str(), engine.Render("sample", ctx.GetContext()));
    }

    {
        doc->article_id = 0;
        doc->owner_id = 123;
        doc->sub_heading = "";
        ctx.Set("warning", "WARNING: You've gone mad!");

        std::ifstream str(template_base + "/hello-world.out-3.html");
        std::stringstream buffer;
        buffer << str.rdbuf();
        EXPECT_EQ(buffer.str(), engine.Render("sample", ctx.GetContext()));
    }
}
