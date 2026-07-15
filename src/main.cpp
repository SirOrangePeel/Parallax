#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <stdexcept>
#include <chrono>
#include <pthread.h>
#include <bits/stdc++.h>

//Create a Chunk Queue class
//Should have Push, Pop, and Finish (To notify child thread)
//Main thread:
//  use stream.gcount() and stream.read() to look through file
//  start phase 2
//Child Thread:
//  carry a word across chunk boundaries
//  pop data from chunk queue
//  loop through the characters 
//  determine if it matches a token process it if it does 
//  Start another thread to do catagorization and the others 

class ChunkQueue {
    public:
        void push(const std::string &chunk) {
            {
                std::lock_guard<std::mutex> lock{mutex};
                queue.push_back(std::move(chunk));
            }
            cond_var.notify_one();
        }

        bool pop(std::string &out) {
            std::unique_lock<std::mutex> lock{mutex};
            cond_var.wait(lock, [this] { return !queue.empty() || is_finished; });
            if (is_finished) {
                if(!queue.empty()) {
                    for(size_t i = 0; i < queue.size(); i++) {
                        out.append(queue.at(i));
                    }
                }
                
                return true;
            }

            if (!queue.empty()) {
                size_t queueSize = queue.size();
                for(size_t i = 0; i < queueSize; i++) {
                    out.append(queue.at(i));
                }
                queue.erase(queue.begin(), queue.begin() + queueSize);
                queue.shrink_to_fit();
            }

            return false;
        }

        void finished() {
            {
                std::lock_guard<std::mutex> lock(mutex);
                is_finished = true;
            }
            cond_var.notify_all();
        }

    private:
        std::vector<std::string> queue;
        bool is_finished = false;
        std::mutex mutex;
        std::condition_variable cond_var;
};

std::mutex mtx;
std::condition_variable cv;
bool complete = false;

void Tokenize(ChunkQueue &queue, std::vector<std::string> &tokens) {
    pthread_setname_np(pthread_self(), "Tokenizer");
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::string container;
        while (true) {
            std::string buffer;          // fresh each time
            bool done = queue.pop(buffer);
            // Basic for now, when proof of concept it complete
            // This will be changed to something more akin to what real complilers use
            for (char c : buffer) {
                switch (c) {
                    case ' ':
                        if(!container.empty()) {
                            tokens.push_back(std::move(container));
                        }
                        break;
                    case '\n':
                        if(!container.empty()) {
                            tokens.push_back(std::move(container));
                        }
                        tokens.push_back("NEWLINE");
                        break;
                    case '\t':
                        if(!container.empty()) {
                            tokens.push_back(std::move(container));
                        }
                        tokens.push_back("TAB");
                        break;
                    default:
                        container += c;
                        break;
                }
            }
            if (done) break;
        }
        complete = true;
    }
    cv.notify_one();
}

void FileRead(ChunkQueue &queue, std::vector<std::string> &chunks) {
    pthread_setname_np(pthread_self(), "FileReader");
    for (std::string str : chunks) {
        queue.push(str);
    }
    queue.finished();
}

int main(/*int argc, char* argv[]*/) {
    pthread_setname_np(pthread_self(), "Main");

    // if (argc != 2) {
    //     throw std::runtime_error("Invalid number of arguments");
    // }

    // path = argc[1];
    // std::vector<char> buffer;
    // thread
    // {
    //     constexpr std::size_t read_size = 4096;
    //     std::ifstream stream = path.data();
    //     stream.exceptions(std::ios_base::badbit);

    //     if (not stream) {
    //         throw std::ios_base::failure("File does not exist");
    //     }

    //     std::uintmax_t file_size = std::filesystem::file_size(path);
    //     buffer.resize(file_size);
    //     std::copy(  std::istreambuf_iterator<char>(stream),
    //                 std::istreambuf_iterator<char>(),
    //                 buffer.begin() );

    // }

    std::vector<std::string> chunks = {
        "#include <iostream>\n",
        "#include <vector>\n",
        "#include <string>\n",
        "\n",
        "int main() {\n",
        "    std::vector<int> data = {1, 2, 3, 4, 5};\n",
        "    int sum = 0;\n",
        "\n",
        "    for (int val : data) {\n",
        "        sum += val;\n",
        "    }\n",
        "\n",
        "    std::cout << \"Sum: \" << sum << std::endl;\n",
        "\n",
        "    if (sum > 10) {\n",
        "        std::cout << \"Sum exceeds threshold\" << std::endl;\n",
        "    } else {\n",
        "        std::cout << \"Sum within limits\" << std::endl;\n",
        "    }\n",
        "\n",
        "    return 0;\n",
        "}\n"
    };

    ChunkQueue chunkQueue;

    std::vector<std::string> tokens;

    std::thread tokenizer(Tokenize, std::ref(chunkQueue), std::ref(tokens));
    std::thread fileReader(FileRead, std::ref(chunkQueue), std::ref(chunks));

    tokenizer.join();
    fileReader.join();

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return complete; });

    for(std::string str : tokens) 
        std::cout << str << std::endl;
    std::cout << "Tokens Found: " << tokens.size() << std::endl;

    return 0;
}