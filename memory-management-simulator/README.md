# Memory Management and Virtual Memory Simulator

## Project Overview
This project implements a comprehensive memory management simulator that demonstrates virtual memory concepts including paging, page tables, Translation Lookaside Buffer (TLB), and demand paging with a backing store. The simulator models the translation of virtual addresses to physical addresses and tracks important memory management statistics.

## Description
This program simulates a virtual memory management system with the following key components:

### Core Features:
- **Virtual Address Translation**: Converts 16-bit virtual addresses to physical addresses
- **Page Table Management**: Maintains mapping between virtual pages and physical frames
- **TLB Implementation**: 16-entry Translation Lookaside Buffer for fast address translation
- **Demand Paging**: Pages are loaded from backing store only when accessed (page fault)
- **FIFO Replacement**: Both TLB and physical memory use First-In-First-Out replacement
- **Performance Monitoring**: Tracks page faults, TLB hits, and calculates hit/miss rates

### System Specifications:
- **Page Size**: 256 bytes per page
- **Address Space**: 16-bit virtual addresses (65,536 possible addresses)
- **Physical Memory**: 256 frames × 256 bytes = 65,536 bytes total
- **Page Table**: 256 entries (one per possible page)
- **TLB Size**: 16 entries with FIFO replacement
- **Backing Store**: Binary file containing all pages for demand paging

### Memory Architecture:
- **Virtual Address Format**: 16 bits total
  - Upper 8 bits: Page number (0-255)
  - Lower 8 bits: Offset within page (0-255)
- **Physical Address**: Frame number × 256 + offset
- **Address Translation Process**:
  1. Extract page number and offset from virtual address
  2. Check TLB for page-to-frame mapping
  3. If TLB miss, check page table
  4. If page fault, load page from backing store
  5. Update TLB with new mapping
  6. Calculate physical address and retrieve value

## Setup Requirements
- Make sure `gcc` or `g++` is installed on your computer before compiling.

## Compilation Instructions
- Compile the C++ program using the command:

  ```bash
  g++ memory_management.cpp -o memory_management
  ```

- This will create an executable file named `memory_management`.

## Running the Program
- Execute the program using:

  ```bash
  ./memory_management
  ```

## Input Files

### addresses.txt
Contains 1000 virtual addresses to be translated, one per line. Each address is a 16-bit integer representing a virtual memory location that needs to be translated to a physical address.

Sample addresses:
```
16916
62493
30198
53683
40185
...
```

### BACKING_STORE.bin
Binary file containing 256 pages of data (256 bytes each). This file serves as the secondary storage from which pages are loaded into physical memory on demand when page faults occur.

## Running the Program
- Execute the program using:

  ```bash
  ./memory_management
  ```

## Output Files and Results

### sample_output.txt
The program generates comprehensive output showing address translations and performance statistics:

#### Address Translation Log:
For each virtual address, shows:
- Virtual Address: Original 16-bit address
- Physical Address: Translated physical memory location  
- Value: Signed byte value stored at that location

Sample translation output:
```
Virtual Address: 16916 Physical Address: 20 Value: 0
Virtual Address: 62493 Physical Address: 285 Value: 0
Virtual Address: 30198 Physical Address: 758 Value: 29
Virtual Address: 53683 Physical Address: 947 Value: 108
Virtual Address: 40185 Physical Address: 1273 Value: 0
...
```

#### Performance Statistics:
Based on the actual simulation run with 1000 addresses:
```
Total Translated Addresses = 1000
Page Faults = 244
Page Fault Rate = 0.244000 (24.4%)
TLB Hits = 54  
TLB Hit Rate = 0.054000 (5.4%)
```

### Performance Analysis:
- **Page Fault Rate**: 24.4% indicates that 244 out of 1000 virtual addresses resulted in page faults
- **TLB Hit Rate**: 5.4% shows that only 54 addresses found their translations in the TLB
- **Memory Efficiency**: The relatively high page fault rate demonstrates the importance of locality of reference in real applications
- **TLB Effectiveness**: The low TLB hit rate suggests the address pattern has poor temporal locality

## Implementation Details

### Address Translation Algorithm:
1. **Parse Virtual Address**: Extract page number (upper 8 bits) and offset (lower 8 bits)
2. **TLB Lookup**: Check if page-to-frame mapping exists in TLB
3. **Page Table Lookup**: If TLB miss, check page table for valid mapping
4. **Page Fault Handling**: If page not in memory, load from backing store
5. **Memory Update**: Place page in next available frame (FIFO replacement)
6. **TLB Update**: Add new mapping to TLB (FIFO replacement)
7. **Address Calculation**: Compute physical address and retrieve value

### Data Structures:
- **Page Table**: Array of 256 integers mapping virtual pages to physical frames
- **TLB**: Array of 16 structures containing page-frame pairs
- **Physical Memory**: 65,536-byte array representing main memory
- **Backing Store**: Binary file interface for demand paging

### Replacement Policies:
- **Physical Memory**: FIFO replacement when all 256 frames are occupied
- **TLB**: FIFO replacement when all 16 entries are occupied
- **Page Faults**: Always load from backing store on first access to a page