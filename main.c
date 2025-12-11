/*
 * CPU Scheduling Algorithms Project
 * ==================================
 * This program demonstrates CPU scheduling algorithms:
 *   - 1 Preemptive algorithm (to be chosen)
 *   - 2 Non-preemptive algorithms (to be chosen)
 *
 * Features:
 *   - Dynamic input from file (no hardcoded values)
 *   - Handles idle time scenarios
 *   - Displays Gantt chart and metrics
 *
 * Input file format (CSV):
 *   PID,Arrival_Time,Burst_Time,Priority
 *   (Priority is optional)
 */

#include "functions.h"

// Function to display the main menu
void display_menu()
{
    printf("\n");
    printf("========================================\n");
    printf("     CPU SCHEDULING ALGORITHMS\n");
    printf("========================================\n");
    printf("[1] Preemptive Algorithm  (Round Robin)\n");
    printf("    (e.g., SRTF / Preemptive Priority / Round Robin)\n");
    printf("\n");
    printf("[2] Non-Preemptive Algorithm 1(Modified FCFS with Aging)\n");
    printf("    (e.g., FCFS / SJF / Non-preemptive Priority)\n");
    printf("\n");
    printf("[3] Non-Preemptive Algorithm 2\n");
    printf("    (e.g., FCFS / SJF / Non-preemptive Priority)\n");
    printf("\n");
    printf("[4] Run All Algorithms\n");
    printf("[5] Display Loaded Processes\n");
    printf("[6] Reload Processes from File\n");
    printf("[0] Exit\n");
    printf("========================================\n");
    printf("Enter your choice: ");
}

// Function to display loaded processes
void display_loaded_processes(Process processes[], int n)
{
    if (n <= 0)
    {
        printf("\nNo processes loaded.\n");
        return;
    }

    printf("\n===== LOADED PROCESSES =====\n");
    printf("+-----+----------+-------+----------+\n");
    printf("| PID |  Arrival | Burst | Priority |\n");
    printf("+-----+----------+-------+----------+\n");

    for (int i = 0; i < n; i++)
    {
        printf("| %3d | %8d | %5d | %8d |\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority);
    }

    printf("+-----+----------+-------+----------+\n");
    printf("Total: %d processes\n", n);
}

// Run a specific algorithm
void run_algorithm(Process original[], int n, int algorithm_choice)
{
    if (n <= 0)
    {
        printf("\nError: No processes loaded. Please load processes from file first.\n");
        return;
    }

    // Create a working copy to preserve original data
    Process working[MAX_PROCESSES];
    copy_processes(original, working, n);
    reset_processes(working, n);

    // Gantt chart storage
    GanttBlock gantt[MAX_PROCESSES * 10]; // Extra space for context switches
    int gantt_size = 0;

    // Run the selected algorithm
    switch (algorithm_choice)
    {
    case 1:
        printf("\n===== PREEMPTIVE ALGORITHM =====\n");
        preemptive_algorithm(working, n, gantt, &gantt_size);
        break;
    case 2:
        printf("\n===== MODIFIED FCFS WITH AGING =====\n");
        modified_FCFS_with_aging(working, n, gantt, &gantt_size);
        break;
    case 3:
        printf("\n===== NON-PREEMPTIVE ALGORITHM 2 =====\n");
        non_preemptive_algorithm_2(working, n, gantt, &gantt_size);
        break;
    default:
        printf("\nInvalid algorithm choice.\n");
        return;
    }

    // Display results if algorithm was implemented
    if (gantt_size > 0)
    {
        display_gantt_chart(gantt, gantt_size);
        display_results(working, n);
    }
}

int main()
{
    Process processes[MAX_PROCESSES];
    int process_count = 0;
    char filename[MAX_LINE_LENGTH];
    int choice;

    // Initial file load
    printf("========================================\n");
    printf("     CPU SCHEDULING ALGORITHMS\n");
    printf("========================================\n");
    printf("\nEnter input filename: ");
    scanf("%s", filename);

    process_count = read_processes_from_file(filename, processes);

    if (process_count > 0)
    {
        printf("\nSuccessfully loaded %d processes from '%s'\n", process_count, filename);
        sort_by_arrival(processes, process_count); // Sort by arrival time initially
    }
    else if (process_count == 0)
    {
        printf("\nWarning: File '%s' contains no valid process data.\n", filename);
    }
    // If process_count < 0, error message already printed by read function

    // Main menu loop
    do
    {
        display_menu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        case 2:
        case 3:
            run_algorithm(processes, process_count, choice);
            break;

        case 4:
            // Run all algorithms
            printf("\n============ RUNNING ALL ALGORITHMS ============\n");
            for (int i = 1; i <= 3; i++)
            {
                run_algorithm(processes, process_count, i);
                printf("\n------------------------------------------------\n");
            }
            break;

        case 5:
            display_loaded_processes(processes, process_count);
            break;

        case 6:
            // Reload from file
            printf("\nEnter input filename: ");
            scanf("%s", filename);
            process_count = read_processes_from_file(filename, processes);
            if (process_count > 0)
            {
                printf("\nSuccessfully loaded %d processes from '%s'\n", process_count, filename);
                sort_by_arrival(processes, process_count);
            }
            else if (process_count == 0)
            {
                printf("\nWarning: File '%s' contains no valid process data.\n", filename);
            }
            break;

        case 0:
            printf("\nExiting program. Goodbye!\n");
            break;

        default:
            printf("\nInvalid choice. Please try again.\n");
        }

    } while (choice != 0);

    return 0;
}
