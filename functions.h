#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================
// CONSTANTS
// ============================================
#define MAX_PROCESSES 100
#define MAX_LINE_LENGTH 256

// ============================================
// DATA STRUCTURES
// ============================================
typedef struct
{
    int pid;             // Process ID
    int arrival_time;    // Time when process arrives
    int burst_time;      // Total CPU time needed
    int priority;        // Priority (lower number = higher priority, optional)
    int remaining_time;  // Time remaining (used for preemptive algorithms)
    int completion_time; // Time when process finishes
    int turnaround_time; // Completion time - Arrival time
    int waiting_time;    // Turnaround time - Burst time
    int started;         // Flag: has the process started? (0 = no, 1 = yes)
    int completed;       // Flag: is the process done? (0 = no, 1 = yes)
} Process;

// Structure to track Gantt chart entries (including idle times)
typedef struct
{
    int pid;        // Process ID (-1 for idle)
    int start_time; // Start time of this block
    int end_time;   // End time of this block
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
void modified_FCFS_with_aging(Process processes[], int n, GanttBlock gantt[], int *gantt_size);
void non_preemptive_algorithm_2(Process processes[], int n, GanttBlock gantt[], int *gantt_size);
// Options: FCFS, SJF, Non-preemptive Priority

// ============================================
// FUNCTION IMPLEMENTATIONS
// ============================================

// --- File Input ---
int read_processes_from_file(const char *filename, Process processes[])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file '%s'\n", filename);
        return -1;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];

    // Skip header line if present (optional: check if first line is header)
    // fgets(line, MAX_LINE_LENGTH, file);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && count < MAX_PROCESSES)
    {
        // Expected format: PID, Arrival_Time, Burst_Time, Priority
        // Priority is optional - defaults to 0 if not provided
        int pid, arrival, burst, priority = 0;

        int fields = sscanf(line, "%d,%d,%d,%d", &pid, &arrival, &burst, &priority);

        if (fields >= 3)
        { // At minimum: PID, Arrival, Burst
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
void reset_processes(Process processes[], int n)
{
    for (int i = 0; i < n; i++)
    {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
        processes[i].started = 0;
        processes[i].completed = 0;
    }
}

// Copy process array (to preserve original data)
void copy_processes(Process source[], Process dest[], int n)
{
    for (int i = 0; i < n; i++)
    {
        dest[i] = source[i];
    }
}

// Sort by arrival time (ascending)
void sort_by_arrival(Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (processes[j].arrival_time > processes[j + 1].arrival_time)
            {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Sort by burst time (ascending) - for SJF
void sort_by_burst(Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (processes[j].burst_time > processes[j + 1].burst_time)
            {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Sort by priority (ascending - lower number = higher priority)
void sort_by_priority(Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (processes[j].priority > processes[j + 1].priority)
            {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// --- Calculation & Display ---

// Calculate average waiting time and turnaround time
void calculate_metrics(Process processes[], int n, float *avg_wt, float *avg_tat)
{
    int total_wt = 0;
    int total_tat = 0;

    for (int i = 0; i < n; i++)
    {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        total_wt += processes[i].waiting_time;
        total_tat += processes[i].turnaround_time;
    }

    *avg_wt = (float)total_wt / n;
    *avg_tat = (float)total_tat / n;
}

// Display process table with results
void display_results(Process processes[], int n)
{
    float avg_wt, avg_tat;
    calculate_metrics(processes, n, &avg_wt, &avg_tat);
    printf("\n");
    printf("+-----+----------+-------+----------+------------+------------+----------+\n");
    printf("| PID |  Arrival | Burst | Priority | Completion | Turnaround |  Waiting |\n");
    printf("+-----+----------+-------+----------+------------+------------+----------+\n");

    for (int i = 0; i < n; i++)
    {
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

    
    calculate_metrics(processes, n, &avg_wt, &avg_tat);
    printf("\nAverage Waiting Time: %.2f\n", avg_wt);
    printf("Average Turnaround Time: %.2f\n", avg_tat);
}

// Display Gantt chart (including idle times)
void display_gantt_chart(GanttBlock gantt[], int gantt_size)
{
    if (gantt_size == 0)
    {
        printf("\nNo Gantt chart data to display.\n");
        return;
    }

    printf("\n===== GANTT CHART =====\n\n");

    // Top border
    printf(" ");
    for (int i = 0; i < gantt_size; i++)
    {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4)
            width = 4;
        for (int j = 0; j < width; j++)
            printf("-");
        printf(" ");
    }
    printf("\n|");

    // Process IDs
    for (int i = 0; i < gantt_size; i++)
    {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4)
            width = 4;
        if (gantt[i].pid == -1)
        {
            // Idle time
            printf("%*s|", width, "IDLE");
        }
        else
        {
            printf(" P%-*d|", width - 2, gantt[i].pid);
        }
    }
    printf("\n ");

    // Bottom border
    for (int i = 0; i < gantt_size; i++)
    {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4)
            width = 4;
        for (int j = 0; j < width; j++)
            printf("-");
        printf(" ");
    }
    printf("\n");

    // Time markers
    printf("%d", gantt[0].start_time);
    for (int i = 0; i < gantt_size; i++)
    {
        int width = (gantt[i].end_time - gantt[i].start_time) * 2;
        if (width < 4)
            width = 4;
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
void preemptive_algorithm(Process processes[], int n, GanttBlock gantt[], int *gantt_size)
{
    printf("\n===== PREEMPTIVE ROUND ROBIN ALGORITHM =====\n");

    int quantum;
    printf("Enter time quantum: ");
    scanf("%d", &quantum);
    if (quantum <= 0)
    {
        printf("Invalid quantum, defaulting to 1.\n");
        quantum = 1;
    }

    int time = 0;
    int completed = 0;

    // Start at earliest arrival (avoid initial negative idle if arrivals start later)
    int earliest_arrival = processes[0].arrival_time;
    for (int i = 1; i < n; i++)
    {
        if (processes[i].arrival_time < earliest_arrival)
            earliest_arrival = processes[i].arrival_time;
    }
    if (time < earliest_arrival)
        time = earliest_arrival;

    int queue[MAX_PROCESSES * 10];
    int front = 0, rear = 0;
    int in_queue[MAX_PROCESSES] = {0};

    *gantt_size = 0;

    while (completed < n)
    {
        // 1. Enqueue all processes that have already arrived
        for (int i = 0; i < n; i++)
        {
            if (!processes[i].completed &&
                processes[i].arrival_time <= time &&
                processes[i].remaining_time > 0 &&
                !in_queue[i])
            {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }

        // 2. If no one is ready, CPU idle until next arrival
        if (front == rear)
        {
            int next_arrival = 1000000000;
            for (int i = 0; i < n; i++)
            {
                if (!processes[i].completed &&
                    processes[i].arrival_time > time &&
                    processes[i].arrival_time < next_arrival)
                {
                    next_arrival = processes[i].arrival_time;
                }
            }

            if (next_arrival == 1000000000)
                break; // no more work

            // Add/extend IDLE block in Gantt chart
            if (*gantt_size > 0 &&
                gantt[*gantt_size - 1].pid == -1 &&
                gantt[*gantt_size - 1].end_time == time)
            {
                // extend existing idle block
                gantt[*gantt_size - 1].end_time = next_arrival;
            }
            else
            {
                gantt[*gantt_size].pid = -1; 
                gantt[*gantt_size].start_time = time;
                gantt[*gantt_size].end_time = next_arrival;
                (*gantt_size)++;
            }

            printf("[IDLE] Time %d -> %d\n", time, next_arrival);
            time = next_arrival;
            continue;
        }

        // 3. Dequeue next process
        int idx = queue[front++];
        in_queue[idx] = 0;
        Process *p = &processes[idx];

        if (p->completed || p->remaining_time <= 0)
            continue;

        if (!p->started)
            p->started = 1;

        int start_time = time; 

        int run_for = (p->remaining_time < quantum) ? p->remaining_time : quantum;
        if (run_for <= 0)
            continue;

        // 4. Gantt handling: merge with previous block if same PID and contiguous
        if (*gantt_size > 0 &&
            gantt[*gantt_size - 1].pid == p->pid &&
            gantt[*gantt_size - 1].end_time == time)
        {
            // extend previous block
            gantt[*gantt_size - 1].end_time += run_for;
        }
        else
        {
            // new time block
            gantt[*gantt_size].pid = p->pid;
            gantt[*gantt_size].start_time = time;
            gantt[*gantt_size].end_time = time + run_for;
            (*gantt_size)++;
        }

        printf("[P%d] runs from %d to %d (remaining before run: %d)\n",
               p->pid, time, time + run_for, p->remaining_time);

        time += run_for;
        p->remaining_time -= run_for;

        // 5. After advancing time, enqueue ONLY NEW arrivals that arrived during (start_time, time]
        for (int i = 0; i < n; i++)
        {
            if (!processes[i].completed &&
                processes[i].arrival_time > start_time &&
                processes[i].arrival_time <= time &&
                processes[i].remaining_time > 0 &&
                !in_queue[i])
            {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }

        // 6. Not Finished? *insert_megamind_meme*
        if (p->remaining_time == 0 && !p->completed)
        {
            p->completed = 1;
            p->completion_time = time;
            completed++;
            printf("     [P%d completed at time %d]\n", p->pid, time);
        }
        else if (p->remaining_time > 0)
        {
            // Not finished: re-enqueue once at the back
            queue[rear++] = idx;
            in_queue[idx] = 1;
        }
    }

    printf("\nAll processes reached end of Round Robin loop at time %d\n", time);
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

/*
 * MODIFIED FCFS WITH AGING AND BURST-TIME AWARENESS
 * ==================================================
 * This algorithm addresses standard FCFS weaknesses:
 *
 * 1. AGING MECHANISM: Processes gain priority as they wait
 *    - Each time unit waited increases effective priority
 *    - Prevents indefinite waiting (starvation)
 *
 * 2. BURST-TIME AWARENESS: Shorter jobs get slight preference
 *    - When multiple processes have similar wait times
 *    - Reduces convoy effect without full preemption
 *
 * 3. DYNAMIC SCHEDULING: Re-evaluates at each completion
 *    - Not purely first-come (like standard FCFS)
 *    - Balances fairness with efficiency
 *
 * SCORING FORMULA:
 * score = (wait_time * AGING_WEIGHT) - (burst_time * BURST_WEIGHT) - (priority * PRIORITY_WEIGHT)
 *
 * Higher score = Higher selection priority
 */

#define AGING_WEIGHT 2.0    // How much waiting time matters
#define BURST_WEIGHT 0.5    // How much job length matters
#define PRIORITY_WEIGHT 3.0 // How much original priority matters

void modified_FCFS_with_aging(Process processes[], int n, GanttBlock gantt[], int *gantt_size)
{
    int current_time = 0;
    int completed = 0;
    *gantt_size = 0;

    printf("\n===== Modified FCFS with Aging Algorithm =====\n");
    printf("Aging Weight: %.1f | Burst Weight: %.1f | Priority Weight: %.1f\n",
           AGING_WEIGHT, BURST_WEIGHT, PRIORITY_WEIGHT);

    while (completed < n)
    {
        // Find all processes that have arrived by current_time
        int best_idx = -1;
        double best_score = -999999.0;

        for (int i = 0; i < n; i++)
        {
            if (!processes[i].completed && processes[i].arrival_time <= current_time)
            {
                // Calculate dynamic score based on:
                // 1. How long the process has been waiting (aging)
                // 2. How short the burst time is (efficiency)
                // 3. Original priority value (urgency)

                int wait_time = current_time - processes[i].arrival_time;

                double score = (wait_time * AGING_WEIGHT) - (processes[i].burst_time * BURST_WEIGHT) - (processes[i].priority * PRIORITY_WEIGHT);

                // Tie-breaker: if scores are very close, use arrival time (pure FCFS)
                if (best_idx == -1 || score > best_score + 0.001)
                {
                    best_score = score;
                    best_idx = i;
                }
                else if (fabs(score - best_score) <= 0.001)
                {
                    // Scores essentially equal - use arrival time as tie-breaker
                    if (processes[i].arrival_time < processes[best_idx].arrival_time)
                    {
                        best_score = score;
                        best_idx = i;
                    }
                }
            }
        }

        if (best_idx == -1)
        {
            // No process available - CPU idle
            int next_arrival = 999999;
            for (int i = 0; i < n; i++)
            {
                if (!processes[i].completed && processes[i].arrival_time > current_time)
                {
                    if (processes[i].arrival_time < next_arrival)
                    {
                        next_arrival = processes[i].arrival_time;
                    }
                }
            }

            if (next_arrival != 999999)
            {
                // Add idle block to Gantt chart
                gantt[*gantt_size].pid = -1;
                gantt[*gantt_size].start_time = current_time;
                gantt[*gantt_size].end_time = next_arrival;
                (*gantt_size)++;

                printf("[IDLE] Time %d -> %d (waiting for next arrival)\n",
                       current_time, next_arrival);
                current_time = next_arrival;
            }
        }
        else
        {
            // Execute the selected process to completion
            Process *p = &processes[best_idx];

            int wait_time = current_time - p->arrival_time;
            printf("[P%d] Start: %d | Waited: %d | Burst: %d | Score: %.2f\n",
                   p->pid, current_time, wait_time, p->burst_time, best_score);

            // Add to Gantt chart
            gantt[*gantt_size].pid = p->pid;
            gantt[*gantt_size].start_time = current_time;
            gantt[*gantt_size].end_time = current_time + p->burst_time;
            (*gantt_size)++;

            // Update process
            current_time += p->burst_time;
            p->completion_time = current_time;
            p->completed = 1;
            completed++;

            printf("     Complete: %d\n", current_time);
        }
    }

    printf("\nAll processes completed at time %d\n", current_time);
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
void non_preemptive_algorithm_2(Process processes[], int n, GanttBlock gantt[], int *gantt_size)
{
    /*
     * SHORTEST JOB FIRST (SJF) - Non-Preemptive
     * ==========================================
     * Selection Criteria: Process with the smallest burst_time
     * Tie-breaker: If burst times are equal, use arrival_time (FCFS)
     *
     * Characteristics:
     * - Optimal for minimizing average waiting time
     * - Non-preemptive: once a process starts, it runs to completion
     * - May cause starvation for longer processes if short ones keep arriving
     */

    int current_time = 0;
    int completed = 0;
    *gantt_size = 0;

    printf("\n===== SHORTEST JOB FIRST (SJF) - Non-Preemptive =====\n");

    while (completed < n)
    {
        int selected = -1;
        int shortest_burst = 999999;

        // Find process with shortest burst time among those that have arrived
        for (int i = 0; i < n; i++)
        {
            if (!processes[i].completed && processes[i].arrival_time <= current_time)
            {
                // Select if burst time is shorter
                if (processes[i].burst_time < shortest_burst)
                {
                    shortest_burst = processes[i].burst_time;
                    selected = i;
                }
                // Tie-breaker: same burst time -> pick earlier arrival (FCFS)
                else if (processes[i].burst_time == shortest_burst && selected != -1)
                {
                    if (processes[i].arrival_time < processes[selected].arrival_time)
                    {
                        selected = i;
                    }
                }
            }
        }

        if (selected == -1)
        {
            // No process available - CPU is idle
            // Find the next process to arrive
            int next_arrival = 999999;
            for (int i = 0; i < n; i++)
            {
                if (!processes[i].completed && processes[i].arrival_time > current_time)
                {
                    if (processes[i].arrival_time < next_arrival)
                    {
                        next_arrival = processes[i].arrival_time;
                    }
                }
            }

            if (next_arrival != 999999)
            {
                // Add IDLE block to Gantt chart
                gantt[*gantt_size].pid = -1;
                gantt[*gantt_size].start_time = current_time;
                gantt[*gantt_size].end_time = next_arrival;
                (*gantt_size)++;

                printf("[IDLE] Time %d -> %d (waiting for next arrival)\n",
                       current_time, next_arrival);
                current_time = next_arrival;
            }
        }
        else
        {
            // Execute the selected process to completion (non-preemptive)
            Process *p = &processes[selected];

            int wait_time = current_time - p->arrival_time;
            printf("[P%d] Start: %d | Arrival: %d | Waited: %d | Burst: %d (shortest available)\n",
                   p->pid, current_time, p->arrival_time, wait_time, p->burst_time);

            // Add to Gantt chart
            gantt[*gantt_size].pid = p->pid;
            gantt[*gantt_size].start_time = current_time;
            gantt[*gantt_size].end_time = current_time + p->burst_time;
            (*gantt_size)++;

            // Update time and mark process as completed
            current_time += p->burst_time;
            p->completion_time = current_time;
            p->completed = 1;
            completed++;

            printf("     Completed at time %d\n", current_time);
        }
    }

    printf("\nAll processes completed at time %d\n", current_time);
}

#endif // FUNCTIONS_H
