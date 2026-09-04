#include "main.hpp"

std::string TokensToASM(std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for(size_t i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::_ret) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::_intLit) {
                if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::_semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }
    
    return output.str();
}

// Currently getting stuck in an infinite loop
void Tokenization(ChunkQueue &queue, std::vector<Token> &tokens) {
    pthread_setname_np(pthread_self(), "Tokenizer");
    tokenization tokenizer;

    while (true) {
        std::string buffer;        
        bool done = queue.pop(buffer);
        
        tokenizer.setSrc(buffer);
        tokenizer.Tokenize(tokens);
         
        if (done) break;
    }
}

void FileRead(ChunkQueue &queue, const std::string &path) {
    pthread_setname_np(pthread_self(), "FileReader");

    std::ifstream stream;
    try {
            stream = std::ifstream(path.data());
            stream.exceptions(std::ios_base::badbit);

            if (not stream) {
                throw std::ios_base::failure("File does not exist");
            }
    } catch (const std::ios_base::failure& e) {
        std::cout << "Caught an ios_base::failure.\n" 
                << e.what() << '\n'
                << "Error code: " << e.code() << '\n';
    }

    // Reading file
    constexpr std::size_t read_size = 256;
    std::vector<char> buffer(read_size);
    while (stream.read(buffer.data(), read_size) || stream.gcount() > 0) {
        std::streamsize byteCount = stream.gcount();
        std::string chunk(buffer.begin(), buffer.begin() + byteCount);
        queue.push(std::move(chunk));
    }

    stream.close();
    queue.finished();
}

int main(int argc, char* argv[]) {
    pthread_setname_np(pthread_self(), "Main");
    // Ensure src file has been provided
    try {
        if (argc != 2) {
            std::cout << "Throwing error" << std::endl;
            throw std::runtime_error("Invalid Number of Arguments");
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n"
                  << "Proper Usage: \n" 
                  << "./parallax {FileNameHere}.px" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string path = argv[1];
    
    std::vector<Token> tokens;
    ChunkQueue queue;

    std::thread tokenizer(Tokenization, std::ref(queue), std::ref(tokens));
    std::thread fileReader(FileRead, std::ref(queue), std::ref(path));

    tokenizer.join();
    fileReader.join();

    std::cout << "Number of Tokens: " << tokens.size() << std::endl;

    std::string _asm = TokensToASM(tokens);
    std::ofstream outStream("src/build/out.asm");
    if (outStream.is_open()) {
        outStream << _asm;
        outStream.close();
    }

    return EXIT_SUCCESS;
}