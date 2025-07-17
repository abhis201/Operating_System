/**************************************************
 * Author: Abhishek Singh
 * Assignment: Process Scheduling Simulator
 * Description: Simulates a scheduler with real-time and interactive processes,
 *              ensuring priority-based CPU execution, FCFS disk handling, and
 *              sequential ordering of CPU → Disk → Terminal.
 **************************************************/

 #include <iostream>
 #include <fstream>
 #include <queue>
 #include <vector>
 #include <map>
 #include <sstream>
 #include <cstdlib>
 
 using namespace std;
 
 // Define symbolic constants
 const int REAL_TIME = 1;
 const int INTERACTIVE = 2;
 
 enum Status { READY, RUNNING, WAITING, TERMINATED };
 
 // Define event types
 const int EVENT_ARRIVAL = 0;
 const int EVENT_CPU_COMPLETION = 1;
 const int EVENT_DISK_COMPLETION = 2;
 const int EVENT_TTY_COMPLETION = 3;
 
 struct Process {
     int id;
     int type;
     int arrivalTime;
     int deadline;  // Only used for real-time processes
     vector<pair<string, int>> requests;  // Resource requests (e.g., "CPU", 100)
     Status status;
     int remainingCpuTime;  // For preemption tracking
 
     // Default constructor (needed for map initialization)
     Process() : id(0), type(INTERACTIVE), arrivalTime(0), deadline(-1),
                 status(READY), remainingCpuTime(0) {}
 
     Process(int pid, int t, int at, int dl = -1)
         : id(pid), type(t), arrivalTime(at), deadline(dl),
           status(READY), remainingCpuTime(0) {}
 };
 
 struct Event {
     int time;
     int type;      // 0: Arrival, 1: CPU Completion, 2: Disk Completion, 3: TTY Completion
     int processId;
     
     // Sorting: earlier time first; if equal, lower event type has priority (CPU < Disk < TTY)
     bool operator<(const Event &e) const {
         if (time == e.time)
             return type > e.type;  // Lower type value has higher priority.
         return time > e.time;
     }
 };
 
 // Global simulation variables
 priority_queue<Event> eventQueue;
 queue<int> realTimeQueue, interactiveQueue, diskQueue;
 map<int, Process> processTable;
 int currentRunningProcess = -1;   // Process currently using the CPU (-1 if idle)
 int cpuStartTime = 0;             // When the current CPU burst started
 int scheduledCPUCompletionTime = 0; // Expected completion time of the running CPU burst
 
 // Utilization and statistics
 double totalCPUBusyTime = 0;      // Total time CPU was busy
 double totalDiskBusyTime = 0;     // Total time Disk was busy
 int totalDiskAccesses = 0;        // Total disk accesses (count)
 double totalDiskAccessTime = 0;   // Sum of (disk completion time - disk request arrival time)
 map<int, int> diskQueueEnterTime; // For each process, time at which its disk request entered the disk queue
 
 int totalSimulationTime = 0;      // Time at simulation end
 int realTimeCompleted = 0;
 int interactiveCompleted = 0;
 int realTimeMissed = 0;           // Count of real-time processes that missed their deadline
 
 // -------------------- CPU Scheduling and Preemption -------------------- //
 
 // Schedule next process for CPU execution at time 'currentTime'
 void scheduleCPU(int currentTime) {
     // If a process is already running, do nothing.
     if (currentRunningProcess != -1)
         return;
  
     if (!realTimeQueue.empty()) {
         int pid = realTimeQueue.front();
         realTimeQueue.pop();
         Process &proc = processTable[pid];
         proc.status = RUNNING;
         if (proc.remainingCpuTime == 0 && !proc.requests.empty())
             proc.remainingCpuTime = proc.requests[0].second;
         currentRunningProcess = pid;
         cpuStartTime = currentTime;
         scheduledCPUCompletionTime = currentTime + proc.remainingCpuTime;
         eventQueue.push(Event{ scheduledCPUCompletionTime, EVENT_CPU_COMPLETION, pid });
     }
     else if (!interactiveQueue.empty()) {
         int pid = interactiveQueue.front();
         interactiveQueue.pop();
         Process &proc = processTable[pid];
         proc.status = RUNNING;
         if (proc.remainingCpuTime == 0 && !proc.requests.empty())
             proc.remainingCpuTime = proc.requests[0].second;
         currentRunningProcess = pid;
         cpuStartTime = currentTime;
         scheduledCPUCompletionTime = currentTime + proc.remainingCpuTime;
         eventQueue.push(Event{ scheduledCPUCompletionTime, EVENT_CPU_COMPLETION, pid });
     }
 }
  
 // -------------------- Event Handlers -------------------- //
 
 // Handle process arrival events
 void handleArrival(Event e) {
     Process &p = processTable[e.processId];
     cout << e.time << " ms: Process " << p.id << " ("
          << (p.type == REAL_TIME ? "REAL-TIME" : "INTERACTIVE") << ") started" << endl;
     
     // Enqueue based on process type
     if (p.type == REAL_TIME) {
         realTimeQueue.push(p.id);
         // Preempt a running interactive process if needed
         if (currentRunningProcess != -1 && processTable[currentRunningProcess].type == INTERACTIVE) {
             int runningPid = currentRunningProcess;
             Process &runningProc = processTable[runningPid];
             int timeSpent = e.time - cpuStartTime;
             runningProc.remainingCpuTime = (timeSpent < runningProc.remainingCpuTime) ?
                                             (runningProc.remainingCpuTime - timeSpent) : 0;
             totalCPUBusyTime += (e.time - cpuStartTime);
             runningProc.status = READY;
             interactiveQueue.push(runningPid);
             currentRunningProcess = -1;
         }
     }
     else {
         interactiveQueue.push(p.id);
     }
     
     scheduleCPU(e.time);
 }
  
 // Handle CPU completion events
 void handleCPUCompletion(Event e) {
     if (currentRunningProcess != e.processId)
         return;  // Stale event due to preemption
     
     Process &p = processTable[e.processId];
     totalCPUBusyTime += (e.time - cpuStartTime);
     p.remainingCpuTime = 0;
     p.requests.erase(p.requests.begin());
     
     // Check if process is finished; for REAL_TIME, check deadline.
     if (p.requests.empty()) {
         p.status = TERMINATED;
         if (p.type == REAL_TIME) {
             if (e.time > p.deadline) {
                 cout << e.time << " ms: Process " << p.id << " terminated (deadline missed)" << endl;
                 realTimeMissed++;
             } else {
                 cout << e.time << " ms: Process " << p.id << " terminated" << endl;
                 realTimeCompleted++;
             }
         } else {
             cout << e.time << " ms: Process " << p.id << " terminated" << endl;
             interactiveCompleted++;
         }
     }
     else {
         // Process next request
         if (p.requests[0].first == "DISK") {
             diskQueue.push(p.id);
             totalDiskAccesses++;
             diskQueueEnterTime[p.id] = e.time;
             if (diskQueue.size() == 1)
                 eventQueue.push(Event{ e.time + p.requests[0].second, EVENT_DISK_COMPLETION, p.id });
         }
         else if (p.requests[0].first == "TTY") {
             eventQueue.push(Event{ e.time + p.requests[0].second, EVENT_TTY_COMPLETION, p.id });
         }
     }
     currentRunningProcess = -1;
     scheduleCPU(e.time);
 }
  
 // Handle disk completion events
 void handleDiskCompletion(Event e) {
     Process &p = processTable[e.processId];
     int diskServiceTime = p.requests[0].second;
     totalDiskBusyTime += diskServiceTime;
     
     // Calculate disk access time (waiting + service)
     if (auto it = diskQueueEnterTime.find(p.id); it != diskQueueEnterTime.end()) {
         totalDiskAccessTime += (e.time - it->second);
         diskQueueEnterTime.erase(it);
     }
     
     diskQueue.pop();
     if (!diskQueue.empty()) {
         int nextPid = diskQueue.front();
         Process &nextProc = processTable[nextPid];
         diskQueueEnterTime[nextPid] = e.time;
         eventQueue.push(Event{ e.time + nextProc.requests[0].second, EVENT_DISK_COMPLETION, nextPid });
     }
     p.requests.erase(p.requests.begin());
     if (p.requests.empty()) {
         p.status = TERMINATED;
         if (p.type == REAL_TIME) {
             if (e.time > p.deadline) {
                 cout << e.time << " ms: Process " << p.id << " terminated (deadline missed)" << endl;
                 realTimeMissed++;
             } else {
                 cout << e.time << " ms: Process " << p.id << " terminated" << endl;
                 realTimeCompleted++;
             }
         } else {
             cout << e.time << " ms: Process " << p.id << " terminated" << endl;
             interactiveCompleted++;
         }
     }
     else {
         if (p.requests[0].first == "CPU") {
             if (p.type == REAL_TIME)
                 realTimeQueue.push(p.id);
             else
                 interactiveQueue.push(p.id);
             scheduleCPU(e.time);
         }
         else if (p.requests[0].first == "TTY") {
             eventQueue.push(Event{ e.time + p.requests[0].second, EVENT_TTY_COMPLETION, p.id });
         }
     }
 }
  
 // Handle TTY completion events (each process uses its own terminal)
 void handleTTYCompletion(Event e) {
     Process &p = processTable[e.processId];
     p.requests.erase(p.requests.begin());
     if (p.requests.empty()) {
         p.status = TERMINATED;
         if (p.type == REAL_TIME) {
             if (e.time > p.deadline) {
                 cout << e.time << " ms: Process " << p.id << " terminated (deadline missed)" << endl;
                 realTimeMissed++;
             } else {
                 cout << e.time << " ms: Process " << p.id << " terminated" << endl;
                 realTimeCompleted++;
             }
         } else {
             cout << e.time << " ms: Process " << p.id << " terminated" << endl;
             interactiveCompleted++;
         }
     }
     else {
         if (p.requests[0].first == "CPU") {
             if (p.type == REAL_TIME)
                 realTimeQueue.push(p.id);
             else
                 interactiveQueue.push(p.id);
             scheduleCPU(e.time);
         }
         else if (p.requests[0].first == "DISK") {
             diskQueue.push(p.id);
             totalDiskAccesses++;
             diskQueueEnterTime[p.id] = e.time;
             if (diskQueue.size() == 1)
                 eventQueue.push(Event{ e.time + p.requests[0].second, EVENT_DISK_COMPLETION, p.id });
         }
         else if (p.requests[0].first == "TTY") {
             eventQueue.push(Event{ e.time + p.requests[0].second, EVENT_TTY_COMPLETION, p.id });
         }
     }
 }
  
 // -------------------- Simulation Loop -------------------- //
  
 void runSimulation() {
     while (!eventQueue.empty()) {
         Event e = eventQueue.top();
         eventQueue.pop();
         totalSimulationTime = e.time;
         
         switch (e.type) {
             case EVENT_ARRIVAL: 
                 handleArrival(e);
                 break;
             case EVENT_CPU_COMPLETION: 
                 handleCPUCompletion(e);
                 break;
             case EVENT_DISK_COMPLETION: 
                 handleDiskCompletion(e);
                 break;
             case EVENT_TTY_COMPLETION:
                 handleTTYCompletion(e);
                 break;
         }
     }
     
     // Summary Report
     cout << "\nSummary Report:\n";
     cout << "Real-Time processes completed: " << realTimeCompleted << endl;
     cout << "Real-Time processes missed deadline: " 
          << (realTimeCompleted + realTimeMissed == 0 ? 0.0 : (realTimeMissed * 100.0) / (realTimeCompleted + realTimeMissed)) << "%" << endl;
     cout << "Interactive processes completed: " << interactiveCompleted << endl;
     cout << "Total disk accesses: " << totalDiskAccesses << endl;
     double avgDiskAccessTime = (totalDiskAccesses > 0) ? (totalDiskAccessTime / totalDiskAccesses) : 0;
     cout << "Average disk access time: " << avgDiskAccessTime << " ms" << endl;
     cout << "Total simulation time: " << totalSimulationTime << " ms" << endl;
     double cpuUtil = (totalSimulationTime > 0) ? (totalCPUBusyTime / totalSimulationTime * 100) : 0;
     double diskUtil = (totalSimulationTime > 0) ? (totalDiskBusyTime / totalSimulationTime * 100) : 0;
     cout << "CPU Utilization: " << cpuUtil << "%" << endl;
     cout << "Disk Utilization: " << diskUtil << "%" << endl;
 }
  
 // -------------------- Input Processing -------------------- //
  
 // Reads the input file (provided via redirection) and populates the process table and event queue.
 void loadProcesses(const string &filename) {
     ifstream infile(filename);
     if (!infile) {
         cerr << "Error: Unable to open file " << filename << endl;
         exit(1);
     }
     
     string line;
     int pid = 0;
     
     while (getline(infile, line)) {
         // Skip empty lines
         if (line.empty()) continue;
         
         istringstream iss(line);
         string word;
         if (!(iss >> word)) continue;
         
         if (word == "REAL-TIME" || word == "INTERACTIVE") {
             int startTime;
             if (!(iss >> startTime)) continue;
             int type = (word == "REAL-TIME") ? REAL_TIME : INTERACTIVE;
             int deadline = -1;
             
             if (type == REAL_TIME) {
                 // Next non-empty line should contain the DEADLINE information.
                 while(getline(infile, line)) {
                     if (!line.empty()) break;
                 }
                 istringstream deadlineStream(line);
                 string temp;
                 deadlineStream >> temp >> deadline;
             }
             
             pid++; // Assign a new process ID starting from 1
             processTable[pid] = Process(pid, type, startTime, deadline);
             eventQueue.push(Event{ startTime, EVENT_ARRIVAL, pid });
         }
         else if (word == "CPU" || word == "DISK" || word == "TTY") {
             int duration;
             if (!(iss >> duration)) continue;
             // Ensure a process exists before adding requests
             if (pid > 0)
                 processTable[pid].requests.push_back(make_pair(word, duration));
         }
     }
     
     infile.close();
 }
  
 // -------------------- Main -------------------- //
  
 int main() {
     cout << "Starting Process Scheduling Simulation..." << endl;
     
     // Load processes from input file (use input redirection: ./a.out < input.txt)
     loadProcesses("input.txt");
     
     // Run the simulation
     runSimulation();
     
     return 0;
 }