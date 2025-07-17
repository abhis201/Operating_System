# Process Scheduling Simulator

## Project Overview
This project implements a comprehensive process scheduling simulator that models a multi-tasking operating system with priority-based CPU scheduling, resource management, and real-time constraints. The simulator demonstrates key operating system concepts including process scheduling algorithms, resource allocation, and system performance metrics.

## Description
This program simulates process scheduling in a system with real-time and interactive processes. The simulator implements:

### Key Features:
- **Priority-based CPU Scheduling**: Real-time processes have absolute priority over interactive processes
- **Preemptive Scheduling**: Higher priority processes can preempt lower priority ones
- **Multi-resource Management**: Handles CPU, Disk, and TTY (Terminal) resources
- **Real-time Deadline Tracking**: Monitors and reports deadline misses for real-time processes
- **FCFS Disk Scheduling**: First-Come-First-Served scheduling for disk requests
- **Sequential Resource Access**: Enforces CPU → Disk → Terminal ordering for process requests
- **Comprehensive Statistics**: Tracks utilization, completion rates, and deadline performance

### Architecture:
- **Event-driven Simulation**: Uses priority queue for efficient event scheduling
- **Process State Management**: Tracks READY, RUNNING, WAITING, and TERMINATED states
- **Resource Utilization Tracking**: Monitors CPU and disk usage throughout simulation
- **Performance Metrics**: Calculates utilization percentages and average access times

## Compilation and Execution

### Compile:
```bash
g++ -o process_scheduler process_scheduler.cpp
```
or
```bash
g++ -std=c++17 -o process_scheduler process_scheduler.cpp
```

### Run with input redirection:
```bash
./process_scheduler < input.txt
```

## Input Format
The simulator accepts a structured input format for defining processes and their resource requirements:

### Process Declaration:
- `REAL-TIME <arrival_time>` - Declares a real-time process arriving at specified time
- `INTERACTIVE <arrival_time>` - Declares an interactive process arriving at specified time
- `DEADLINE <absolute_time>` - Sets deadline for real-time processes (must follow REAL-TIME declaration)

### Resource Requests (sequential order):
- `CPU <time>` - CPU burst time in milliseconds
- `DISK <time>` - Disk access time in milliseconds  
- `TTY <time>` - Terminal/TTY access time in milliseconds

### Example Input:
```
INTERACTIVE 12000    # Interactive process arrives at 12000ms
CPU 100             # Requests CPU for 100ms
TTY 5000            # Then TTY for 5000ms
CPU 100             # Then CPU for 100ms
DISK 10             # Then disk for 10ms
CPU 20              # Finally CPU for 20ms

REAL-TIME 12000     # Real-time process arrives at 12000ms
DEADLINE 13000      # Must complete by 13000ms
CPU 30              # Requests CPU for 30ms
```

## Output
The simulator provides detailed execution logs and comprehensive performance statistics:

### Execution Log:
- Process start notifications with timestamps
- Process termination events
- Real-time scheduling decisions and preemption events

### Summary Report:
- **Process Completion Statistics**: Count of completed real-time and interactive processes
- **Deadline Performance**: Percentage of real-time processes that missed their deadlines
- **Resource Utilization**: CPU and disk utilization percentages
- **Access Metrics**: Total disk accesses and average disk access time
- **Simulation Duration**: Total simulation time

### Sample Output:
```
Starting Process Scheduling Simulation...
12000 ms: Process 1 (INTERACTIVE) started
12000 ms: Process 2 (REAL-TIME) started
12030 ms: Process 2 terminated
17230 ms: Process 1 terminated

Summary Report:
Real-Time processes completed: 1
Real-Time processes missed deadline: 0%
Interactive processes completed: 1
Total disk accesses: 1
Average disk access time: 10 ms
Total simulation time: 17230 ms
CPU Utilization: 1.27684%
Disk Utilization: 0.0580383%
```

## Implementation Details

### Scheduling Algorithm:
- **Real-time Priority**: Real-time processes always preempt interactive processes
- **FCFS within Priority**: Processes of same priority follow First-Come-First-Served
- **Resource Ordering**: Enforces sequential access pattern (CPU → Disk → TTY)

### Data Structures:
- **Event Queue**: Priority queue for efficient time-ordered event processing
- **Process Queues**: Separate queues for real-time, interactive, and disk scheduling
- **Process Table**: Maps process IDs to process control blocks

### Performance Metrics:
- **CPU Utilization**: (Total CPU busy time / Total simulation time) × 100
- **Disk Utilization**: (Total disk busy time / Total simulation time) × 100
- **Deadline Miss Rate**: (Missed deadlines / Total real-time processes) × 100
