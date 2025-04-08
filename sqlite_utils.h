#ifndef SQLITE_UTILS_H
#define SQLITE_UTILS_H

#include <sqlite3.h>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <string>
#include <cstdint>

inline std::vector<char> generate_blob(int number, size_t blob_size) {
    std::vector<char> blob(blob_size, 0);
    uint64_t num = number;
    std::memcpy(blob.data(), &num, sizeof(num));
    return blob;
}

inline void configure_connection(sqlite3* db) {
    const char* pragmas = 
        "PRAGMA journal_mode = OFF;"
        "PRAGMA synchronous = 0;"
        "PRAGMA locking_mode = EXCLUSIVE;"
        "PRAGMA cache_size = -2000;"
        "PRAGMA temp_store = MEMORY;";
        
    char* errMsg = nullptr;
    if (sqlite3_exec(db, pragmas, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string msg = "DB config failed: ";
        msg += errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error(msg);
    }
}

inline sqlite3* open_database(const std::string& filename) {
    sqlite3* db;
    if (sqlite3_open_v2(filename.c_str(), &db, 
                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, nullptr) != SQLITE_OK) {
        std::string msg = "Cannot open database: ";
        msg += sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error(msg);
    }
    return db;
}

inline void insert_blobs(const std::string& db_file, int start_number, int count, size_t blob_size) {
    sqlite3* db = open_database(db_file);
    configure_connection(db);
    
    const char* sql = "INSERT INTO blobs (data) VALUES (?)";
    sqlite3_stmt* stmt = nullptr;
    
    try {
        // Prepare statement
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Prepare failed: " + 
                                   std::string(sqlite3_errmsg(db)));
        }

        // Start transaction
        if (sqlite3_exec(db, "BEGIN IMMEDIATE TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Begin transaction failed: " + 
                                   std::string(sqlite3_errmsg(db)));
        }

        // Insert all blobs
        for (int i = 0; i < count; ++i) {
            auto blob = generate_blob(start_number + i, blob_size);
            
            if (sqlite3_bind_blob(stmt, 1, blob.data(), blob.size(), SQLITE_STATIC) != SQLITE_OK ||
                sqlite3_step(stmt) != SQLITE_DONE) {
                throw std::runtime_error("Insert failed: " + 
                                       std::string(sqlite3_errmsg(db)));
            }
            sqlite3_reset(stmt);
        }

        // Commit transaction
        if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Commit failed: " + 
                                   std::string(sqlite3_errmsg(db)));
        }
    }
    catch (...) {
        // Cleanup on error
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw;
    }

    // Cleanup
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

#endif // SQLITE_UTILS_H