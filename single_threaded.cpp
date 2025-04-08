#include "sqlite_utils.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            throw std::runtime_error("Usage: " + std::string(argv[0]) + 
                " <count> <blob_size> <database_file>");
        }

        int count = std::stoi(argv[1]);
        size_t blob_size = std::stoul(argv[2]);
        std::string db_file = argv[3];

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

        for (int i = 1; i <= count; ++i) {
            auto blob = generate_blob(i, blob_size);
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
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}