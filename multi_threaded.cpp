#include "sqlite_utils.h"
#include <iostream>
#include <thread>
#include <vector>

void insert_blobs(int thread_id, int start, int count, size_t blob_size, 
                 const std::string& db_file) {
    try {
        sqlite3* db = open_database(db_file);
        configure_connection(db);

        const char* sql = "INSERT INTO blobs (data) VALUES (?)";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Prepare failed: " + 
                std::string(sqlite3_errmsg(db)));
        }

        rc = sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Begin transaction failed: " + 
                std::string(sqlite3_errmsg(db)));
        }

        for (int i = 0; i < count; ++i) {
            auto blob = generate_blob(start + i, blob_size);
            sqlite3_bind_blob(stmt, 1, blob.data(), blob.size(), SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                throw std::runtime_error("Insert failed: " + 
                    std::string(sqlite3_errmsg(db)));
            }
            sqlite3_reset(stmt);
        }

        rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Commit failed: " + 
                std::string(sqlite3_errmsg(db)));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    } catch (const std::exception& e) {
        std::cerr << "Thread " << thread_id << ": " << e.what() << std::endl;
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
            threads.emplace_back(insert_blobs, i, current_start, count, 
                               blob_size, db_file);
            current_start += count;
        }

        for (auto& t : threads) {
            t.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}