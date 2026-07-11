#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <filesystem>
#include <future>
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


void Separate() {
    if()
    for (char c : buffer) {
        if (c != ' ')
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        throw std::runtime_error("Invalid number of arguments");
    }

    path = argc[1];
    std::vector<char> buffer;
    thread
    {
        constexpr std::size_t read_size = 4096;
        std::ifstream stream = path.data();
        stream.exceptions(std::ios_base::badbit);

        if (not stream) {
            throw std::ios_base::failure("File does not exist");
        }

        std::uintmax_t file_size = std::filesystem::file_size(path);
        buffer.resize(file_size);
        std::copy(  std::istreambuf_iterator<char>(stream),
                    std::istreambuf_iterator<char>(),
                    buffer.begin() );

    }

    return 0;
}