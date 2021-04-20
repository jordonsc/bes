#pragma once

#include <bes/core.h>

namespace bes::templating {

class TemplateException : public bes::BesException
{
    using bes::BesException::BesException;
};

class MalformedExpressionException : public TemplateException
{
   public:
    MalformedExpressionException(std::string const& expr) : TemplateException("Malformed expression: " + expr) {}
    MalformedExpressionException(std::string const& expr, std::string const& msg)
        : TemplateException("Malformed expression: " + expr + "; " + msg)
    {}
};

class MalformedSymbolException : public MalformedExpressionException
{
    using MalformedExpressionException::MalformedExpressionException;
};

class MissingEndTagException : public TemplateException
{
    using TemplateException::TemplateException;
};

class UnknownTypeException : public TemplateException
{
    using TemplateException::TemplateException;
};

class IndexErrorException : public TemplateException
{
    using TemplateException::TemplateException;
};

class DuplicateBlockNodeException : public IndexErrorException
{
    using IndexErrorException::IndexErrorException;
};

class DuplicateMacroNodeException : public IndexErrorException
{
    using IndexErrorException::IndexErrorException;
};

class ValueErrorException : public TemplateException
{
    using TemplateException::TemplateException;
};

class MissingContextException : public TemplateException
{
    using TemplateException::TemplateException;
};

class MissingTemplateException : public TemplateException
{
    using TemplateException::TemplateException;
};

class CircularInheritanceException : public TemplateException
{
    using TemplateException::TemplateException;
};

}  // namespace bes::templating
