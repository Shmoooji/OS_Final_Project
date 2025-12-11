# CPU Scheduling Algorithms Project

## TL;DR - Quick Start

```bash
gcc main.c -o scheduler
./scheduler
# Enter: output/processes.txt
```

**Your task**: Implement your assigned function in `functions.h` (bottom of file).

| Member | Function | Algorithm | Status |
|--------|----------|-----------|--------|
| A | `preemptive_algorithm()` | Round Robin | COMPLETE |
| B | `non_preemptive_algorithm_1()` | Modified FCFS with Aging | COMPLETE |
| C | `non_preemptive_algorithm_2()` | SJF | COMPLETE |

**Must do in your algorithm**:
1. Handle idle time → use `gantt[i].pid = -1`
2. Fill Gantt chart → set `pid`, `start_time`, `end_time`
3. Set `processes[i].completion_time` when process finishes
4. Increment `(*gantt_size)++` after each Gantt block

---

## Project Status

| Component | Status | Notes |
|-----------|--------|-------|
| Round Robin (Preemptive) | COMPLETE | Fully functional with time quantum, handles idle time |
| Modified FCFS with Aging | COMPLETE | Uses dynamic scoring for starvation prevention |
| SJF (Non-preemptive) | COMPLETE | Selects shortest burst time, FCFS tie-breaker |
| Core Infrastructure | COMPLETE | File I/O, sorting, display, metrics |
| Test Data | COMPLETE | 2 test files in `output/` folder |

---

## Summary

### Project Structure
```
project/
├── main.c           # Menu system (DONE - don't modify)
├── functions.h      # Utilities + Algorithms implementation
└── output/
    ├── processes.txt           # Test file (normal - no idle gaps)
    └── processes_with_idle.txt # Test file (has idle gaps)
```

### Input File Format (CSV)
```
PID,Arrival_Time,Burst_Time,Priority
```
Example: `1,0,5,2` means Process 1 arrives at 0, needs 5 time units, priority 2.

### Key Structs

**Process** - each process has:
- `pid`, `arrival_time`, `burst_time`, `priority`
- `remaining_time` (for preemptive algorithms)
- `completion_time` (YOU SET THIS)
- `completed` flag (set to 1 when done)

**GanttBlock** - each execution block:
- `pid` (-1 for IDLE)
- `start_time`, `end_time`

### Utility Functions (Already Done)
- `sort_by_arrival()`, `sort_by_burst()`, `sort_by_priority()`
- `display_results()`, `display_gantt_chart()`
- `calculate_metrics()`, `reset_processes()`, `copy_processes()`

---

## Algorithm Implementation Template

### Non-Preemptive (FCFS/SJF/Priority)

```c
void non_preemptive_algorithm_X(Process processes[], int n, GanttBlock gantt[], int *gantt_size) {
    int current_time = 0;
    int completed = 0;
    *gantt_size = 0;

    while (completed < n) {
        int selected = -1;

        // Find available process based on YOUR criteria
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && !processes[i].completed) {
                // FCFS: pick first found
                // SJF: pick smallest burst_time
                // Priority: pick smallest priority number
                selected = i;
                break; // modify this based on algorithm
            }
        }

        if (selected == -1) {
            // IDLE TIME - no process available
            int next_arrival = /* find earliest arrival of non-completed process */;
            gantt[*gantt_size].pid = -1;
            gantt[*gantt_size].start_time = current_time;
            gantt[*gantt_size].end_time = next_arrival;
            (*gantt_size)++;
            current_time = next_arrival;
        } else {
            // Execute process
            gantt[*gantt_size].pid = processes[selected].pid;
            gantt[*gantt_size].start_time = current_time;
            current_time += processes[selected].burst_time;
            gantt[*gantt_size].end_time = current_time;
            (*gantt_size)++;

            processes[selected].completion_time = current_time;
            processes[selected].completed = 1;
            completed++;
        }
    }
}
```

### Preemptive (SRTF/Preemptive Priority)

```c
void preemptive_algorithm(Process processes[], int n, GanttBlock gantt[], int *gantt_size) {
    int current_time = 0;
    int completed = 0;
    int prev_pid = -1;
    *gantt_size = 0;

    while (completed < n) {
        int selected = -1;

        // Find process with shortest remaining_time (or highest priority)
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && !processes[i].completed) {
                // SRTF: pick smallest remaining_time
                // Priority: pick smallest priority number
            }
        }

        if (selected == -1) {
            // IDLE TIME
            // Add idle block, jump to next arrival
        } else {
            // Check context switch (did process change?)
            if (processes[selected].pid != prev_pid) {
                // New Gantt block
                gantt[*gantt_size].pid = processes[selected].pid;
                gantt[*gantt_size].start_time = current_time;
                gantt[*gantt_size].end_time = current_time + 1;
                (*gantt_size)++;
            } else {
                // Extend previous block
                gantt[*gantt_size - 1].end_time++;
            }

            processes[selected].remaining_time--;
            current_time++;
            prev_pid = processes[selected].pid;

            if (processes[selected].remaining_time == 0) {
                processes[selected].completion_time = current_time;
                processes[selected].completed = 1;
                completed++;
                prev_pid = -1;
            }
        }
    }
}
```

---

## Testing

1. **Normal test**: `processes.txt` - no idle time expected
2. **Idle test**: `processes_with_idle.txt` - should show IDLE blocks

---

## Checklist

- [x] Algorithm handles idle time (pid = -1)
- [x] Gantt chart fills correctly
- [x] completion_time set for each process
- [x] Works with both test files
- [x] No hardcoded values
- [x] Round Robin (Preemptive) implemented
- [x] Modified FCFS with Aging implemented
- [x] SJF (Non-preemptive Algorithm 2) implemented

## Implemented Algorithms Details

### 1. Round Robin (Preemptive) - `preemptive_algorithm()`
- Uses configurable time quantum (user input)
- Queue-based scheduling with FIFO order
- Handles process arrivals dynamically
- Merges consecutive Gantt blocks for same process
- Properly handles idle time when no processes are ready

### 2. Modified FCFS with Aging - `non_preemptive_algorithm_1()`
- Uses dynamic scoring formula: `score = (wait_time * 2.0) - (burst_time * 0.5) - (priority * 3.0)`
- Prevents starvation through aging mechanism
- Re-evaluates process selection after each completion
- Tie-breaker uses arrival time (true FCFS fallback)

### 3. SJF - `non_preemptive_algorithm_2()`
- Selects the process with the shortest burst time among available processes
- Tie-breaker: if burst times are equal, uses arrival time (FCFS)
- Non-preemptive: once a process starts, it runs to completion
- Handles idle time when no processes are available
- Optimal for minimizing average waiting time
