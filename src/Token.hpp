#pragma once
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <optional>

enum class TokenType {_ret, _intLit, _semi, _newLine, _int, _equal, _ident};

typedef struct {
    TokenType type;
    std::optional<std::string> varName{};
    std::optional<std::string> value{};
} Token;

std::map<std::string, Token> keywords = {
    {"return", { .type = TokenType::_ret }},
};