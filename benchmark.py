import subprocess
import time
import os
from pathlib import Path
import logging
from concurrent.futures import ProcessPoolExecutor

# Configuration
BLOB_SIZE = 1024
TOTAL_BLOBS = 1000000  # 10x original size
NUM_THREADS = 10
NUM_PROCESSES = 10
DB_DIR = Path("dbs")
BINARIES_DIR = Path("bin")
SRC_DIR = Path(".")

# Path setup
DB_DIR.mkdir(exist_ok=True, parents=True)
BINARIES_DIR.mkdir(exist_ok=True, parents=True)

SINGLE_BIN = BINARIES_DIR / "single_threaded"
MULTI_BIN = BINARIES_DIR / "multi_threaded"

def compile_programs():
    """Compile C++ programs with proper path handling"""
    try:
        # Compile single-threaded
        subprocess.run([
            "g++", "-std=c++17", "-O3", 
            str(SRC_DIR / "single_threaded.cpp"),
            "-o", str(SINGLE_BIN),
            "-lsqlite3"
        ], check=True)
        
        # Compile multi-threaded
        subprocess.run([
            "g++", "-std=c++17", "-O3",
            str(SRC_DIR / "multi_threaded.cpp"),
            "-o", str(MULTI_BIN),
            "-pthread", "-lsqlite3"
        ], check=True)
        
        # Verify binaries exist
        if not SINGLE_BIN.exists():
            raise FileNotFoundError(f"Failed to create {SINGLE_BIN}")
        if not MULTI_BIN.exists():
            raise FileNotFoundError(f"Failed to create {MULTI_BIN}")

    except subprocess.CalledProcessError as e:
        logging.error("Compilation failed. Ensure you have:")
        logging.error("1. g++ installed")
        logging.error("2. SQLite development libraries (libsqlite3-dev)")
        raise
    except FileNotFoundError as e:
        logging.error("Binary verification failed: %s", e)
        raise

def initialize_databases():
    """Create and configure databases with absolute paths"""
    dbs = [
        DB_DIR / "single.db",
        *[DB_DIR / f"multi_{i}.sqlite" for i in range(NUM_THREADS)],
        *[DB_DIR / f"process_{i}.sqlite" for i in range(NUM_PROCESSES)]
    ]

    for db_path in dbs:
        if db_path.exists():
            db_path.unlink()
        
        subprocess.run([
            "sqlite3", str(db_path),
            "PRAGMA journal_mode=OFF;"
            "PRAGMA synchronous=OFF;"
            "PRAGMA locking_mode=EXCLUSIVE;"
            "CREATE TABLE blobs(id INTEGER PRIMARY KEY, data BLOB);"
        ], check=True)

def run_single_threaded():
    """Run single-threaded benchmark"""
    start = time.perf_counter()
    subprocess.run([
        str(SINGLE_BIN), 
        str(TOTAL_BLOBS), 
        str(BLOB_SIZE), 
        str(DB_DIR/"single.db")
    ], check=True)
    return time.perf_counter() - start

def run_multi_threaded():
    """Run multi-threaded benchmark"""
    start = time.perf_counter()
    subprocess.run([
        str(MULTI_BIN),
        str(TOTAL_BLOBS),
        str(BLOB_SIZE),
        str(NUM_THREADS),
        str(DB_DIR/"multi")
    ], check=True)
    return time.perf_counter() - start

def run_multi_process():
    """Run multi-process benchmark using process pool"""
    start = time.perf_counter()
    with ProcessPoolExecutor() as executor:
        futures = []
        per_process = TOTAL_BLOBS // NUM_PROCESSES
        
        for i in range(NUM_PROCESSES):
            db_path = DB_DIR/f"process_{i}.sqlite"
            count = per_process + (1 if i < (TOTAL_BLOBS % NUM_PROCESSES) else 0)
            
            futures.append(executor.submit(
                subprocess.run,
                [
                    str(SINGLE_BIN),
                    str(count),
                    str(BLOB_SIZE),
                    str(db_path)
                ],
                check=True
            ))
        
        # Wait for all processes to complete
        for future in futures:
            future.result()
    
    return time.perf_counter() - start

def main():
    logging.basicConfig(level=logging.INFO)
    
    try:
        logging.info("Compiling programs...")
        compile_programs()
        
        logging.info("Binary locations:")
        logging.info("Single-threaded: %s", SINGLE_BIN.resolve())
        logging.info("Multi-threaded: %s", MULTI_BIN.resolve())

        logging.info("Initializing databases...")
        initialize_databases()

        logging.info("Running benchmarks (total blobs: %d)", TOTAL_BLOBS)
        
        # Single-threaded
        st_time = run_single_threaded()
        logging.info("Single-threaded: %.2f seconds", st_time)
        
        # Multi-threaded
        mt_time = run_multi_threaded()
        logging.info("Multi-threaded: %.2f seconds", mt_time)
        
        # Multi-process
        mp_time = run_multi_process()
        logging.info("Multi-process: %.2f seconds", mp_time)
        
        # Print summary
        print("\nFinal Results:")
        print(f"Single-threaded: {st_time:.2f}s")
        print(f"Multi-threaded: {mt_time:.2f}s")
        print(f"Multi-process: {mp_time:.2f}s")

    except Exception as e:
        logging.error("Benchmark failed: %s", e)
        raise

if __name__ == "__main__":
    main()