#include "expression.h"

using namespace bes::templating::syntax;

/**
 * Creates an Expression from a plain text string.
 */
Expression::Expression(std::string const& str)
{
    raw = str;
    char pos = 0;
    auto parts = bes::templating::Text::splitArgs(str);
    bool neg = false;
    bool no_op = false;
    bool expect_filter = false;

    if (parts.empty()) {
        throw MalformedExpressionException(str, BES_TEMPLATING_INCOMPLETE_EXPR);
    }

    for (auto const& part : parts) {
        if (pos && end_tag && clause != Clause::ELIF) {
            // Only "elif" end tags contain additional parts
            throw MalformedExpressionException(str, "bad use of a closing tag");
        }

        /*
         * Function-style syntax (eg `{{ mymacro(foo, bar) }}`).
         *
         * The parentheses are considered the right side, and there is no operator here so we'll skip the operator
         * phase by incrementing `pos`.
         */
        if (part.length() > 1 && part[0] == '(' && pos == 1) {
            ++pos;
        }

        if (pos == 0) {
            // POS 0: CLAUSE -OR- LEFT VALUE
            if (part == "block") {
                clause = Clause::BLOCK;
                continue;
            } else if (part == "extends") {
                clause = Clause::EXTENDS;
                continue;
            } else if (part == "include") {
                clause = Clause::INCLUDE;
                continue;
            } else if (part == "for") {
                validateClause(neg, str);
                clause = Clause::FOR;
                continue;
            } else if (part == "macro") {
                validateClause(neg, str);
                clause = Clause::MACRO;
                no_op = true;
                continue;
            } else if (part == "if") {
                validateClause(neg, str);
                clause = Clause::IF;
                continue;
            } else if (part == "endblock") {
                validateClause(neg, str);
                clause = Clause::ENDBLOCK;
                end_tag = true;
                continue;
            } else if (part == "endfor") {
                validateClause(neg, str);
                clause = Clause::ENDFOR;
                end_tag = true;
                continue;
            } else if (part == "endmacro") {
                validateClause(neg, str);
                clause = Clause::ENDMACRO;
                end_tag = true;
                continue;
            } else if (part == "endif") {
                validateClause(neg, str);
                clause = Clause::ENDIF;
                end_tag = true;
                continue;
            } else if (part == "elif") {
                validateClause(neg, str);
                clause = Clause::ELIF;
                end_tag = true;
                continue;
            } else if (part == "else") {
                validateClause(neg, str);
                clause = Clause::ELSE;
                end_tag = true;
                continue;
            } else if (part == "not") {
                if (neg) {
                    throw MalformedExpressionException(str, BES_TEMPLATING_DBL_NOT);
                }
                neg = true;
                continue;
            }

            // If not a control keyword, then it must be the left value
            negated = neg;
            left = Symbol(part);
            neg = false;

            // Expressions that skip the operator -
            if (no_op) {
                ++pos;
            }
        } else if (pos == 1) {
            // POS 1: OPERATOR
            if (part == "|") {
                expect_filter = true;
                continue;
            }

            if (expect_filter) {
                filters.push_back(part);
                expect_filter = false;
                continue;
            }

            if (part == "not") {
                if (neg) {
                    throw MalformedExpressionException(str, BES_TEMPLATING_DBL_NOT);
                }
                neg = true;
                continue;
            }

            if (part == "in") {
                // 'in' expressions valid for 'for' or 'if'
                if (op != Operator::NONE || clause == Clause::NONE) {
                    throw MalformedExpressionException(str, "cannot use 'in' keyword here");
                }
                negated = neg;
                op = Operator::IN;
                neg = false;
            } else if (part == "is") {
                // 'is' expressions only valid for 'if'
                if (op != Operator::NONE || clause != Clause::IF) {
                    throw MalformedExpressionException(str, "cannot use 'is' keyword here");
                }
                if (neg) {
                    // 'not' cannot proceed "is" (must be after)
                    throw MalformedExpressionException(str, "'not' must follow 'is', not proceed it");
                }
                op = Operator::IS;
            } else if (part == "==") {
                validateComparisonOperator(neg, str);
                op = Operator::EQUALS;
            } else if (part == "!=") {
                validateComparisonOperator(neg, str);
                op = Operator::NOT_EQUALS;
            } else if (part == ">") {
                validateComparisonOperator(neg, str);
                op = Operator::GT;
            } else if (part == ">=") {
                validateComparisonOperator(neg, str);
                op = Operator::GTE;
            } else if (part == "<") {
                validateComparisonOperator(neg, str);
                op = Operator::LT;
            } else if (part == "<=") {
                validateComparisonOperator(neg, str);
                op = Operator::LTE;
            } else {
                // Must be a control keyword in this position
                throw MalformedExpressionException(str, "expected control keyword, instead got '" + part + "'");
            }
        } else if (pos == 2) {
            // POS 2: RIGHT VALUE
            if (part == "not") {
                // Allowed to 'not' an "is defined", but that's the only acceptable use of 'not' after the control
                // keyword
                if (neg || op != Operator::IS) {
                    throw MalformedExpressionException(str, BES_TEMPLATING_INVALID_NOT);
                }
                neg = true;
                continue;
            } else if (part == "defined") {
                // Must be an "is defined" or "is not defined"
                if (op != Operator::IS) {
                    throw MalformedExpressionException(str, "'defined' must follow 'is'");
                }
                negated = neg;
                op = Operator::IS_DEFINED;
                neg = false;
            } else {
                if (op == Operator::IS) {
                    throw MalformedExpressionException(str, "'is' must be followed by [not] 'defined'");
                }

                right = Symbol(part);
            }
        } else {
            // Should never be a word after the right value
            throw MalformedExpressionException(str, "expression is too long");
        }

        ++pos;
    }

    if (neg) {
        throw MalformedExpressionException("invalid use of 'not' keyword");
    }

    if (expect_filter) {
        throw MalformedExpressionException("expected filter name after | operator");
    }

    // Validation
    switch (op) {
        case Operator::IS:
            throw MalformedExpressionException(str, BES_TEMPLATING_INCOMPLETE_EXPR);
        case Operator::IS_DEFINED:
            if (!right.items.empty()) {
                throw MalformedExpressionException(str, "'is defined' requires no further symbols");
            }
            break;
        case Operator::IN:
            if (right.items.empty()) {
                throw MalformedExpressionException(str, BES_TEMPLATING_INCOMPLETE_EXPR);
            }
            break;
        default:
            break;
    }

    switch (clause) {
        case Clause::NONE:
            if (!left.items.empty() && right.items.empty()) {
                clause = Clause::VALUE;
            } else if (!left.items.empty() && right.symbol_type == Symbol::SymbolType::FUNCTION) {
                clause = Clause::FUNCTION;
            } else {
                throw MalformedExpressionException(str, "null control type");
            }
            break;
        case Clause::BLOCK:
            if (left.items.size() != 1) {
                throw MalformedExpressionException(str, "block clause must have a single name");
            }
            if (left.symbol_type != Symbol::SymbolType::VARIABLE) {
                throw MalformedExpressionException(str, "block name should match variable syntax");
            }
            if (!right.items.empty() || op != Operator::NONE) {
                throw MalformedExpressionException(str, "block clause must contain only a name");
            }
            break;
        case Clause::INCLUDE:
        case Clause::EXTENDS:
            if (left.items.size() != 1) {
                throw MalformedExpressionException(str, "clause must have a single name string literal");
            }
            if (left.symbol_type != Symbol::SymbolType::LITERAL || left.data_type != Symbol::DataType::STRING) {
                throw MalformedExpressionException(str, "include & extends names should be a string literal");
            }
            if (!right.items.empty() || op != Operator::NONE) {
                throw MalformedExpressionException(str, "clause must contain only a name string literal");
            }
            break;
        case Clause::IF:
        case Clause::ELIF:
            if (left.items.empty()) {
                throw MalformedExpressionException(str, BES_TEMPLATING_INCOMPLETE_EXPR);
            }
            break;
        case Clause::FOR:
            if (op != Expression::Operator::IN) {
                throw MalformedSymbolException(str, "for loops require an 'in' clause");
            }

            if (left.symbol_type != Symbol::SymbolType::VARIABLE) {
                throw MalformedSymbolException(str, "for loops require a variable on the left");
            }

            if (right.symbol_type != Symbol::SymbolType::ARRAY && right.symbol_type != Symbol::SymbolType::VARIABLE) {
                throw MalformedSymbolException(str, "for loops require an array or variable on the right");
            }
            break;
        case Clause::MACRO:
            if (op != Expression::Operator::NONE) {
                throw MalformedSymbolException(str, "macro block cannot have an operator");
            }

            if (left.symbol_type != Symbol::SymbolType::VARIABLE) {
                throw MalformedSymbolException(str, "macro blocks require a variable on the left");
            }

            if (right.symbol_type != Symbol::SymbolType::FUNCTION) {
                throw MalformedSymbolException(str, "macro block expects function syntax");
            }

            // All elements in the function array must be a "variable" type (may have zero args, though)
            for (auto& item : right.items) {
                auto sym = std::any_cast<syntax::Symbol>(item);
                if (sym.symbol_type != Symbol::SymbolType::VARIABLE) {
                    throw MalformedSymbolException(str, "all macro block arguments must be variables");
                }
            }

        default:
            break;
    }
}

void Expression::validateComparisonOperator(bool neg, std::string const& str) const
{
    if (op != Operator::NONE || ((clause != Clause::IF) && (clause != Clause::ELIF))) {
        throw MalformedExpressionException(str, "cannot use comparison keywords outside of 'if' statements");
    }
    if (neg) {
        throw MalformedExpressionException(str, "do not use 'not' keywords with comparison operators");
    }
}

/**
 * Validates that the "clause" part of the expression is valid:
 *
 * - Cannot have already been negated (eg "not for ...")
 * - Must not already have a clause defined
 */
void Expression::validateClause(bool neg, std::string const& str) const
{
    if (neg) {
        throw MalformedExpressionException(str, BES_TEMPLATING_INVALID_NOT);
    } else if (clause != Clause::NONE) {
        throw MalformedExpressionException(str, "multiple uses of expression clauses");
    }
}
