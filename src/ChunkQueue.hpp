#include "main.hpp"

class ChunkQueue {
    public:
        // Lock shared queue to push new chunks
        void push(const std::string &chunk) {
            {
                std::lock_guard<std::mutex> lock{mutex};
                queue.push_back(std::move(chunk));
            }
            cond_var.notify_one();
        }

        // Waits until queue is unlocked or the file read is complete
        bool pop(std::string &out) {
            std::unique_lock<std::mutex> lock{mutex};
            cond_var.wait(lock, [this] { return !queue.empty() || is_finished; });

            size_t queueSize = queue.size();

            if (!queue.empty()) {
                for(size_t i = 0; i < queueSize; i++) {
                    out.append(queue.at(i));
                }
                queue.erase(queue.begin(), queue.begin() + queueSize);
                queue.shrink_to_fit();
            }

            return is_finished;
        }

        // Notify the tokenizer that file read is done.
        void finished() {
            {
                std::lock_guard<std::mutex> lock(mutex);
                is_finished = true;
            }
            cond_var.notify_all();
        }

        int getLen() {
            return queue.size();
        }

    private:
        std::vector<std::string> queue;
        bool is_finished = false;
        std::mutex mutex;
        std::condition_variable cond_var;
};