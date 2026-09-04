#include "Token.hpp"

class tokenization {
    public:
        // Need to add chunk border checking
        void Tokenize(std::vector<Token> &tokens) {
            // While there is still stuff left in the chunk
            while(peek().has_value()) {
                char c = peek().value();
                // See if the value is a letter
                if(std::isalpha(c)) {
                    buffer.push_back(consume());
                    // After confirmation, continue until a separator shows up
                    while(peek().has_value() && std::isalnum(peek().value())) {
                        buffer.push_back(consume());
                        
                    }

                    auto search = keywords.find(buffer);
                    if(search != keywords.end()) {
                        tokens.push_back(search->second);
                    } else {
                        tokens.push_back({ .type = TokenType::_ident});
                    }

                    buffer.clear();
                }

                if(peek().has_value() && std::isdigit(c)) {
                    buffer.push_back(consume());
                    
                    while(peek().has_value() && std::isdigit(peek().value())) {
                        buffer.push_back(consume());
                    }

                    tokens.push_back({ .type = TokenType::_intLit, .value = { buffer }});
                    buffer.clear();
                }

                if(peek().has_value() && peek().value() == ';') {
                    tokens.push_back({ .type = TokenType::_semi });
                    
                    buffer.clear();
                }

                consume();
            }
        }

        void setSrc(std::string chunk) {
            src_chunk = chunk;
        }

    private:
        [[nodiscard]] std::optional<char> peek(int offset = 0) const {
            if (chunk_pos + offset >= src_chunk.length()) {
                return {};
            }
            return src_chunk.at(chunk_pos + offset);
        }

        char consume() {
            char c = src_chunk.at(chunk_pos);
            chunk_pos++;
            return c;
        }

        std::string buffer;
        std::string src_chunk;
        int chunk_pos = 0;
};