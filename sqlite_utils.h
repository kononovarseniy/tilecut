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
    int rc = sqlite3_exec(db, pragmas, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = "DB config failed: ";
        msg += errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error(msg);
    }
}

inline sqlite3* open_database(const std::string& filename) {
    sqlite3* db;
    int rc = sqlite3_open_v2(filename.c_str(), &db, 
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        std::string msg = "Cannot open database: ";
        msg += sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error(msg);
    }
    return db;
}

#endif // SQLITE_UTILS_H