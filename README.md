# Parallax
I wanna see if I can make a coding language with the constraint of when it is compiled, it can be done in parallel.

My idea might be kinda crazy, but I want it to tokenize generate and optimize at the same time. I will do this by using fork to create different processes and pass data along using a [Named pipe][https://biendltb.github.io/tech/inter-process-communication-ipc-in-cpp/].


Currently I am working on a multithreaded sender receiver setup for reading the file and tokenizer.