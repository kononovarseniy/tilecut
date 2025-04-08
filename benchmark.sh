#!/bin/bash

# Compile programs
g++ -std=c++11 -O3 -lsqlite3 single_threaded.cpp -o single_threaded
g++ -std=c++11 -O3 -lsqlite3 -pthread multi_threaded.cpp -o multi_threaded

# Initialize databases
initialize_db() {
    local db_file=$1
    sqlite3 "$db_file" "CREATE TABLE IF NOT EXISTS blobs (id INTEGER PRIMARY KEY, data BLOB);"
}

echo "Initializing databases..."
initialize_db "single.db"
for i in {0..9}; do
    initialize_db "multi_$i.sqlite"
    initialize_db "process_$i.sqlite"
done

# Configuration
BLOB_SIZE=1024
TOTAL_BLOBS=10000
NUM_THREADS=10

echo "Scenario 1: Single-threaded"
time ./single_threaded $TOTAL_BLOBS $BLOB_SIZE single.db

echo "Scenario 2: Multi-threaded"
time ./multi_threaded $TOTAL_BLOBS $BLOB_SIZE $NUM_THREADS multi

echo "Scenario 3: Multi-process"
start_time=$(date +%s%3N)
for i in {0..9}; do
    ./single_threaded 1000 $BLOB_SIZE "process_$i.sqlite" &
done
wait
end_time=$(date +%s%3N)
echo "Time taken: $((end_time - start_time)) milliseconds"

# Cleanup (optional)
# rm -f single.db multi_*.sqlite process_*.sqlite