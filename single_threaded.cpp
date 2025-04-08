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

        insert_blobs(db_file, 1, count, blob_size);
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}