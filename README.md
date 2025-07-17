# Operating System Simulators

This repository contains two comprehensive Operating System simulation projects implemented in C++. These simulators demonstrate fundamental OS concepts through practical implementations.

## 📁 Project Structure

### 1. Memory Management Simulator
**Directory:** [`memory-management-simulator/`](./memory-management-simulator/)

A virtual memory management system that simulates address translation using:
- **TLB (Translation Lookaside Buffer)** - Fast address translation cache
- **Page Table** - Virtual to physical address mapping
- **Backing Store** - Secondary storage simulation
- **Physical Memory** - Main memory simulation

**Key Features:**
- Virtual address translation (16-bit logical to physical addresses)
- TLB hit/miss handling with LRU replacement
- Page fault handling with backing store access
- Statistics tracking (TLB hit rate, page fault rate)

[📖 View Memory Management README](./memory-management-simulator/README.md)

### 2. Process Scheduling Simulator
**Directory:** [`process-scheduling-simulator/`](./process-scheduling-simulator/)

A CPU scheduling simulator implementing multiple scheduling algorithms:
- **FCFS (First-Come, First-Served)**
- **SJF (Shortest Job First)**
- **Priority Scheduling**
- **Round Robin**

**Key Features:**
- Process arrival and burst time simulation
- Gantt chart generation
- Performance metrics calculation (waiting time, turnaround time)
- Comparative analysis of different scheduling algorithms

[📖 View Process Scheduling README](./process-scheduling-simulator/README.md)

## 🚀 Quick Start

### Prerequisites
- C++ compiler (g++ or clang++)
- macOS/Linux/Windows with C++ development environment

### Building and Running

1. **Memory Management Simulator:**
   ```bash
   cd memory-management-simulator/
   g++ -o memory_management memory_management.cpp
   ./memory_management addresses.txt
   ```

2. **Process Scheduling Simulator:**
   ```bash
   cd process-scheduling-simulator/
   g++ -o process_scheduler process_scheduler.cpp
   ./process_scheduler input.txt
   ```

## 📊 Educational Value

These simulators provide hands-on experience with:
- **Memory Management:** Virtual memory, paging, TLB operations
- **CPU Scheduling:** Different scheduling algorithms and their trade-offs
- **Performance Analysis:** Metrics calculation and optimization strategies
- **System Programming:** Low-level memory operations and data structures

## 🎯 Learning Outcomes

After working with these simulators, you'll understand:
- How virtual memory systems translate addresses
- The impact of different scheduling algorithms on system performance
- Memory hierarchy and caching mechanisms
- Process management and resource allocation strategies

## 📝 Documentation

Each project directory contains:
- Detailed README with implementation specifics
- Sample input/output files
- Performance analysis reports
- Build instructions and usage examples

---

**Author:** Abhishek Singh  
**Repository:** [Operating_System](https://github.com/abhis201/Operating_System)
