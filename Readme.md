# Reconstruction Blockhouse

## Overview
This project processes market-by-order (MBO) data from a CSV file and reconstructs the market-by-price (MBP) data. The program is optimized for performance and memory usage.

## Features
- Converts MBO events into MBP-10 formatted snapshots.
- Captures the top 10 levels of bids and asks efficiently.
- Ensures correctness under high-frequency trading (HFT) constraints.

---

## File Description
- `mbo.csv`: Input file containing raw MBO events.
- `mbp_reconstructed.csv`: Output file containing the reconstructed MBP-10 order book.
- `main.cpp` : Source code files.
- `README.md`: This documentation file.

---

## How to Build
To compile the project, use the provided Makefile. Run the following command in the terminal:

```bash
make
```

Or manually compile:

```bash
g++ -O2 -std=c++20 -o reconstruction_blockhouse main.cpp new.cpp -lpsapi
```

This will generate an executable named `reconstruction_blockhouse`.

---

## How to Run
To run the program, use the following command:

```bash
./reconstruction_blockhouse mbo.csv
```

Replace `mbo.csv` with the path to your input CSV file. The output will be written to `mbp_reconstructed.csv`.

---

## Logic and Implementation Details

### 1. Data Structures
- `std::map<long double, int>` for bids and asks store aggregate size per price level.
- `std::set` for `bid_prices` and `ask_prices` efficiently tracks sorted price levels (descending for bids, ascending for asks).
- Separate maps `bid_orders` and `ask_orders` keep track of order counts per price level.

### 2. Parsing Input
- The input is parsed line-by-line using `std::getline()` into an `MBO` struct that holds all relevant fields.

### 3. Book Update Logic
- **`update_book()`**:
  - For `[A]dd` actions: Add size and order to the corresponding price level.
  - For `[C]ancel` actions: Subtract size, and remove the level if size drops to 0.
  - For `[T]rade` actions:
    - Apply the size reduction to the opposite side of the book (e.g., if trade appears on A, reduce B).
    - Ignore `T` actions with side `N`.
  - For `[R]eset` actions: Reset the orderbook state.

### 4. TFC Compression
- Implemented as per specification:
  - A `T` action is immediately followed by `F` and `C`, which are ignored.
  - The effect of this trade is modeled on the opposite side of the book from the trade side.
  - A single MBP entry is logged, with the side swapped appropriately.

### 5. Depth Calculation
- For every update, the depth (distance from top) of the impacted price level is calculated using iterators into the set.

### 6. Output Format
- The CSV output replicates the exact format of `mbp.csv`, including:
  - Metadata
  - Top 10 price levels for both bids and asks
  - Order count and size at each level

### 7. Performance Tools
- Measured execution time using `std::chrono`.
- Memory footprint measured using `psapi` (for Windows) to report:
  - Resident Set Size (RSS)
  - Peak working set (High Water Mark).

---

## Performance Optimizations
1. Disabled sync with `std::cin` and tied streams to speed up I/O.
2. Minimized memory reallocations by avoiding vector use and leveraging maps and sets.
3. Used lazy evaluation (`find_if`) to avoid unnecessary depth computations.
4. Skipped rows where actions don’t affect book state (`F`, `T` with side `N`).

---

## Limitations and Considerations
- Assumes input is well-formed. No malformed line handling is done.
- Currently designed for Windows (due to use of `psapi`). Minor changes needed for Linux (`/proc/self/status`).
- Could benefit from using more cache-friendly structures like flat hash maps or arenas for large-scale HFT deployments.

---

## Possible Enhancements
1. Implement a memory pool allocator to reduce `malloc` overhead.
2. Replace `std::map` and `std::set` with flat maps/sets or skip list for higher performance.
3. Parallelize processing if order independence is guaranteed (e.g., batching).

---


## Assumptions and Shortcomings
- Assumed no Modify or partial cancel orders for the time being
- limited depth to 11 found 10 extra entries, may have made some mistake, orders correctly tracked
- Original code made to track all orders to the last depth, limited depth condition added later

## Performance

Following are the performance statistics:
```bash
Time taken: 0.0366989 s
Memory Usage (RSS): 4980 KB
Peak Memory Usage (HWM): 4980 KB
```
The time taken to parse and produce the MBP csv from the given MBO csv is about 0.03 s to 0.04 s and the memory usage is about 5000 KB.