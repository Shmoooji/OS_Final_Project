#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// CONSTANTS
// ============================================
#define MAX_PROCESSES 100
#define MAX_LINE_LENGTH 256

// ============================================
// DATA STRUCTURES
// ============================================
typedef struct {
    int pid;                // Process ID
    int arrival_time;       // Time when process arrives
    int burst_time;         // Total CPU time needed
    int priority;           // Priority (lower number = higher priority, optional)
    int remaining_time;     // Time remaining (used for preemptive algorithms)
    int completion_time;    // Time when process finishes
    int turnaround_time;    // Completion time - Arrival time
    int waiting_time;       // Turnaround time - Burst time
    int started;            // Flag: has the process started? (0 = no, 1 = yes)
    int completed;          // Flag: is the process done? (0 = no, 1 = yes)
} Process;

// Structure to track Gantt chart entries (including idle times)
typedef struct {
    int pid;                // Process ID (-1 for idle)
    int start_time;         // Start time of this block
    int end_time;           // End time of this block
} GanttBlock;

// ============================================
// FUNCTION PROTOTYPES
// ============================================

// --- File Input ---
int read_processes_from_file(const char *filename, Process processes[]);

// --- Utility Functions ---
void reset_processes(Process processes[], int n);
void copy_processes(Process source[], Process dest[], int n);
void sort_by_arrival(Process processes[], int n);
void sort_by_burst(Process processes[], int n);
void sort_by_priority(Process processes[], int n);

// --- Calculation & Display ---
void calculate_metrics(Process processes[], int n, float *avg_wt, float *avg_tat);
void display_results(Process processes[], int n);
void display_gantt_chart(GanttBlock gantt[], int gantt_size);

// --- Scheduling Algorithms ---
// PREEMPTIVE (choose 1 to implement)
void preemptive_algorithm(Process processes[], int n, GanttBlock gantt[], int *gantt_size);
// Options: SRTF, Preemptive Priority, Round Robin

// NON-PREEMPTIVE (choose 2 to implement)
void non_preemptive_algorithm_1(Process processes[], int n, GanttBlock gantt[], int *gantt_size);
void non_preemptive_algorithm_2(Process processes[], int n, GanttBlock gantt[], int *gantt_size);
// Options: FCFS, SJF, Non-preemptive Priority

// ============================================
// FUNCTION IMPLEMENTATIONS
// ============================================

// --- File Input ---
int read_processes_from_file(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", filename);
        return -1;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];

    // Skip header line if present (optional: check if first line is header)
    // fgets(line, MAX_LINE_LENGTH, file);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && count < MAX_PROCESSES) {
        // Expected format: PID, Arrival_Time, Burst_Time, Priority
        // Priority is optional - defaults to 0 if not provided
        int pid, arrival, burst, priority = 0;

        int fields = sscanf(line, "%d,%d,%d,%d", &pid, &arrival, &burst, &priority);

        if (fields >= 3) {  // At minimum: PID, Arrival, Burst
            processes[count].pid = pid;
            processes[count].arrival_time = arrival;
            processes[count].burst_time = burst;
            processes[count].priority = priority;
            processes[count].remaining_time = burst;
            processes[count].completion_time = 0;
            processes[count].turnaround_time = 0;
            processes[count].waiting_time = 0;
            processes[count].started = 0;
            processes[count].completed = 0;
            count++;
        }
    }

    fclose(file);
    return count;
}

// --- Utility Functions ---

// Reset all calculated fields (use before running an algorithm)
void reset_processes(Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
        processes[i].started = 0;
        processes[i].completed = 0;
    }
}

// Copy process array (to preserve original data)
void copy_processes(Process source[], Process dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = source[i];
    }
}

// Sort by arrival time (ascending)
void sort_by_arrival(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Sort by burst time (ascending) - for SJF
void sort_by_burst(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].burst_time > processes[j + 1].burst_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Sort by priority (ascending - lower number = higher priority)
void sort_by_priority(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].priority > processes[j + 1].priority) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// --- Calculation & Display ---

// Calculate average waiting time and turnaround time
void calculate_metrics(Process processes[], int n, float *avg_wt, float *avg_tat) {
    int total_wt = 0;
    int total_tat = 0;

    for (int i = 0; i < n; i++) {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        total_wt += processes[i].waiting_time;
        total_tat += processes[i].turnaround_time;
    }

    *avg_wt = (float)total_wt / n;
    *avg_tat = (float)total_tat / n;
}

// Display process table with results
void display_results(Process processes[], int n) {
    printf("\n");
    printf("+-----+----------+-------+----------+------------+------------+----------+\n");
    printf("| PID |  Arrival | Burst | Priority | Completion | Turnaround |  Waiting |\n");
    printf("+-----+----------+-------+----------+------------+------------+----------+\n");

    for (int i = 0; i < n; i++) {
        printf("| %3d | %8d | %5d | %8d | %10d | %10d | %8d |\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);
    }

    printf("+-----+----------+-------+----------+------------+------------+----------+\n");

    float avg_wt, avg_tat;
    calculate_metrics(processes, n, &avg_wt, &avg_tat);
    printf("\nAverage Waiting Time: %.2f\n", avg_wt);
    printf("Average Turnaround Time: %.2f\n", avg_tat);
}

// Display Gantt chart (including idle times)
void display_gantt_chart(GanttBlock gantt[], int gantt_size) {
    if (gantt_size == 0) {
        printf("\nNo Gantt chart data to display.\n");
        return;
    }

    printf("\n===== GANTT CHART =====\n\n");

    // Top border
    printf(" ");
    for (int i = 0; i < gantt_size; i++) {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4) width = 4;
        for (int j = 0; j < width; j++) printf("-");
        printf(" ");
    }
    printf("\n|");

    // Process IDs
    for (int i = 0; i < gantt_size; i++) {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4) width = 4;
        if (gantt[i].pid == -1) {
            // Idle time
            printf("%*s|", width, "IDLE");
        } else {
            printf(" P%-*d|", width - 2, gantt[i].pid);
        }
    }
    printf("\n ");

    // Bottom border
    for (int i = 0; i < gantt_size; i++) {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4) width = 4;
        for (int j = 0; j < width; j++) printf("-");
        printf(" ");
    }
    printf("\n");

    // Time markers
    printf("%d", gantt[0].start_time);
    for (int i = 0; i < gantt_size; i++) {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4) width = 4;
        printf("%*d", width + 1, gantt[i].end_time);
    }
    printf("\n");
}

// ============================================
// SCHEDULING ALGORITHMS (STUBS - TO BE IMPLEMENTED)
// ============================================

/*
 * PREEMPTIVE ALGORITHM
 * ---------------------
 * TODO: Implement ONE of the following:
 *   - SRTF (Shortest Remaining Time First)
 *   - Preemptive Priority
 *   - Round Robin (requires time quantum)
 *
 * Remember to:
 *   1. Handle idle time when no process is available
 *   2. Update the Gantt chart array
 *   3. Set completion_time for each process
 */
void preemptive_algorithm(Process processes[], int n, GanttBlock gantt[], int *gantt_size) {
    // TODO: Implement your chosen preemptive algorithm
    printf("\n[!] Preemptive algorithm not yet implemented.\n");
    *gantt_size = 0;
}

/*
 * NON-PREEMPTIVE ALGORITHM 1
 * ---------------------------
 * TODO: Implement ONE of the following:
 *   - FCFS (First Come First Serve)
 *   - SJF (Shortest Job First)
 *   - Non-preemptive Priority
 *
 * Remember to:
 *   1. Handle idle time when no process is available
 *   2. Update the Gantt chart array
 *   3. Set completion_time for each process
 */
void non_preemptive_algorithm_1(Process processes[], int n, GanttBlock gantt[], int *gantt_size) {
    // TODO: Implement your first non-preemptive algorithm
    printf("\n[!] Non-preemptive algorithm 1 not yet implemented.\n");
    *gantt_size = 0;
}

/*
 * NON-PREEMPTIVE ALGORITHM 2
 * ---------------------------
 * TODO: Implement ANOTHER non-preemptive algorithm (different from above)
 *
 * Remember to:
 *   1. Handle idle time when no process is available
 *   2. Update the Gantt chart array
 *   3. Set completion_time for each process
 */
void non_preemptive_algorithm_2(Process processes[], int n, GanttBlock gantt[], int *gantt_size) {
    // TODO: Implement your second non-preemptive algorithm
    printf("\n[!] Non-preemptive algorithm 2 not yet implemented.\n");
    *gantt_size = 0;
}

#endif // FUNCTIONS_H
