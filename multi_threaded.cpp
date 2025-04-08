#include <sqlite3.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <string>

using namespace std;

vector<char> generate_blob(int number, size_t blob_size) {
    vector<char> blob(blob_size, 0);
    uint64_t num = number;
    memcpy(blob.data(), &num, sizeof(num));
    return blob;
}

void configure_connection(sqlite3* db) {
    const char* pragmas = 
        "PRAGMA journal_mode = OFF;"
        "PRAGMA synchronous = 0;"
        "PRAGMA locking_mode = EXCLUSIVE;"
        "PRAGMA cache_size = -2000;"
        "PRAGMA temp_store = MEMORY;";
        
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, pragmas, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        throw runtime_error(string(errMsg));
    }
}

void insert_blobs(int thread_id, int start, int count, size_t blob_size, const string& db_file) {
    try {
        sqlite3* db;
        int rc = sqlite3_open_v2(db_file.c_str(), &db, 
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, nullptr);
        if (rc != SQLITE_OK) {
            throw runtime_error("Open failed: " + string(sqlite3_errmsg(db)));
        }

        configure_connection(db);

        const char* sql = "INSERT INTO blobs (data) VALUES (?)";
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw runtime_error("Prepare failed: " + string(sqlite3_errmsg(db)));
        }

        rc = sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw runtime_error("Begin transaction failed: " + string(sqlite3_errmsg(db)));
        }

        for (int i = 0; i < count; ++i) {
            auto blob = generate_blob(start + i, blob_size);
            sqlite3_bind_blob(stmt, 1, blob.data(), blob.size(), SQLITE_STATIC);
            
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                throw runtime_error("Insert failed: " + string(sqlite3_errmsg(db)));
            }
            sqlite3_reset(stmt);
        }

        rc = sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            throw runtime_error("Commit failed: " + string(sqlite3_errmsg(db)));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    } catch (const exception& e) {
        cerr << "Thread " << thread_id << ": " << e.what() << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <total_blobs> <blob_size> <num_threads> <database_base>\n";
        return 1;
    }

    try {
        int total_blobs = stoi(argv[1]);
        size_t blob_size = stoul(argv[2]);
        int num_threads = stoi(argv[3]);
        string db_base = argv[4];

        vector<thread> threads;
        int per_thread = total_blobs / num_threads;
        int remainder = total_blobs % num_threads;
        int current_start = 1;

        for (int i = 0; i < num_threads; ++i) {
            int count = per_thread + (i < remainder ? 1 : 0);
            if (count <= 0) break;

            string db_file = db_base + "_" + to_string(i) + ".sqlite";
            threads.emplace_back(insert_blobs, i, current_start, count, blob_size, db_file);
            current_start += count;
        }

        for (auto& t : threads) {
            t.join();
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}