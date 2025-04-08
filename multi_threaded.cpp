#include "sqlite_utils.h"
#include <iostream>
#include <thread>
#include <vector>

void thread_task(int thread_id, int start, int count, size_t blob_size, const std::string& db_file) {
    try {
        insert_blobs(db_file, start, count, blob_size);
    }
    catch (const std::exception& e) {
        std::cerr << "Thread " << thread_id << " failed: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            throw std::runtime_error("Usage: " + std::string(argv[0]) + 
                " <total_blobs> <blob_size> <num_threads> <database_base>");
        }

        int total_blobs = std::stoi(argv[1]);
        size_t blob_size = std::stoul(argv[2]);
        int num_threads = std::stoi(argv[3]);
        std::string db_base = argv[4];

        std::vector<std::thread> threads;
        int per_thread = total_blobs / num_threads;
        int remainder = total_blobs % num_threads;
        int current_start = 1;

        for (int i = 0; i < num_threads; ++i) {
            int count = per_thread + (i < remainder ? 1 : 0);
            if (count <= 0) break;

            std::string db_file = db_base + "_" + std::to_string(i) + ".sqlite";
            threads.emplace_back(thread_task, i, current_start, count, blob_size, db_file);
            current_start += count;
        }

        for (auto& t : threads) {
            t.join();
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}