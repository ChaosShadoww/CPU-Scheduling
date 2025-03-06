// Add more header files if required...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PROCESSES 100

/* ========================================================================================*/
// Structure to store process information
typedef struct
{ // Add more variables to store process information if required...
    int pid;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    bool is_completed;

} Process;

/* ========================================================================================*/
// Add more Global variables if required...
Process processes[MAX_PROCESSES];
int num_processes = 0;
int current_time = 0; 
int completed_processes = 0;

/* ========================================================================================*/
// Add more Function prototypes suitable your implementation if required...
void fcfs();                        // First Come First Serve Scheduling
void sjf_non_preemptive();          // Shortest Job First - Non-preemptive
void srt_preemptive();              // Shortest Remaining Time - Preemptive
void round_robin(int time_quantum); // Round Robin Scheduling
void priority_non_preemptive();     // Priority Scheduling - Non-preemptive
void read_processes_from_file(const char *filename); //Read and process the process-related information form `input.txt` file
void calculate_average_times();     // Calculate and display average waiting & turnaround times
void display_results();             // Display scheduling results
void display_process_info();        // Display process details before scheduling
void clear_input_buffer();          // Clears input buffer to prevent invalid input issues
void reset_process_states();        // Resets process variables before each scheduling run
int min(int a, int b);              // Utility function to find the minimum of two numbers

/* ========================================================================================*/
// Main function
int main()
{
    // Add more variables suitable for your implementation if required...
    int choice;
    int time_quantum;
    char input[100]; // Buffer to store user input

    // Read process data from file
    read_processes_from_file("input.txt");

    // User-driven menu
    while (1)
    {
        printf("\n                 CPU Scheduling Algorithms\n");
        printf("|-----------------------------------------------------------|\n");
        printf("|   1. First-Come, First-Served (FCFS)                      |\n");
        printf("|   2. Shortest Job First (SJF) - Nonpreemptive             |\n");
        printf("|   3. Shortest Remaining Time (SRT) - Preemptive           |\n");
        printf("|   4. Round Robin (RR)                                     |\n");
        printf("|   5. Priority Scheduling - Nonpreemptive                  |\n");
        printf("|   0. Exit                                                 |\n");
        printf("|-----------------------------------------------------------|\n");

        printf("\nEnter your choice: ");
        if (!fgets(input, sizeof(input), stdin))
        {
            // Handle EOF (e.g., Ctrl+D)
            printf("\nExiting program.\n\n");
            break;
        }

        // Validate input: check if it's an integer
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Invalid input. Please enter an integer between 0 and 5.\n");
            continue;
        }

        printf("\n");

        switch (choice)
        {
        case 1:
            fcfs();
            break;
        case 2:
            sjf_non_preemptive();
            break;
        case 3:
            srt_preemptive();
            break;
        case 4:
            printf("Enter time quantum for Round Robin Scheduling: ");
            printf("\n");
            scanf("%d", &time_quantum);
            clear_input_buffer(); // Clear the buffer after reading input
            round_robin(time_quantum);
            break;
        case 5:
            priority_non_preemptive();
            break;
        case 0:
            printf("Exiting program.\n\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

/* ========================================================================================*/
// Function to read processes from a file (PLEASE DONOT MODIFY THIS FUNCTION CODE!!!)
void read_processes_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Buffer to hold each line from the file
    char line[256];
    int line_number = 0;

    // Skip the first two lines (header and separator)
    while (line_number < 2 && fgets(line, sizeof(line), file))
    {
        line_number++;
    }

    // Read process data from the file
    while (fgets(line, sizeof(line), file))
    {
        // Skip lines with separators like "====" or "----"
        if (line[0] == '=' || line[0] == '-')
        {
            continue;
        }

        char pid_str[10]; // Buffer to store process name like "P1" or "1"
        int pid, priority, burst_time, arrival_time;

        // Read the process ID and other values
        if (sscanf(line, "%s %d %d %d", pid_str, &burst_time, &priority, &arrival_time) == 4)
        {
            // Extract numeric part from 'P1' or read directly if it's just '1'
            if (sscanf(pid_str, "P%d", &pid) != 1)
            { // Check if it starts with 'P'
                if (sscanf(pid_str, "%d", &pid) != 1)
                { // Otherwise, try reading as a number
                    printf("Invalid process ID format: %s (skipped)\n", pid_str);
                    continue; // Skip invalid process IDs
                }
            }

            // Store the process data
            processes[num_processes].pid = pid;
            processes[num_processes].priority = priority;
            processes[num_processes].burst_time = burst_time;
            processes[num_processes].arrival_time = arrival_time;
            processes[num_processes].remaining_time = burst_time; // Remaining time equals burst time initially
            processes[num_processes].waiting_time = 0;            // Initialize waiting time for aging
            num_processes++;                                      // Increment process count
        }
        else
        {
            printf("Invalid line format: %s (skipped)\n", line); // Handle invalid data lines
        }
    }

    // Close the file after reading
    fclose(file);

    // Display the loaded process information
    display_process_info();
}

/* ========================================================================================*/
// Function to reset process states
void reset_process_states()
{
    // Also reset other process information variables if you have added to the 'Process' structure...
    for (int i = 0; i < num_processes; i++)
    {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].is_completed = false;
    }
}

/* ========================================================================================*/
// Function to clear the input buffer (PLEASE DONOT MODIFY THIS FUNCTION CODE!!!)
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // Consume characters until newline or EOF
}

/* ========================================================================================*/
// Function to display the scheduling statistics
void display_process_info()
{
    // Print fetched values in a table format
    printf("\n\n             Process Scheduling Information\n");
    printf("----------------------------------------------------------\n");
    printf("  | %-5s | %-12s | %-12s | %-12s |\n", "PID", "Burst Time", "Priority", "Arrival Time");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < num_processes; i++)
    {
        printf("  | %-5d | %-12d | %-12d | %-12d |\n",
               processes[i].pid, processes[i].burst_time,
               processes[i].priority, processes[i].arrival_time);
    }

    printf("----------------------------------------------------------\n\n");
}

/* ========================================================================================*/
// Finding minmum of two numbers
int min(int a, int b)
{
    return (a < b) ? a : b;
}

/* ========================================================================================*/
// Calculate Average Waiting Time & Average Turnaround Time
void calculate_average_times()
{
    int total_waiting_time = 0, total_turnaround_time = 0;
    
    // Implementation of rest of your code...

    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);
}

/* ========================================================================================*/
// Display results
void display_results()
{
    // Display Individual Process Turnaround Time & Waiting Time
    printf("\nProcess\t   Waiting Time\t    Turnaround Time\n");
    for (int i = 0; i < num_processes; i++)
    {
        printf("  P%d\t       %d\t\t %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    }

    // Display Average Waiting Time & Average Turnaround Time
    calculate_average_times();
}

/* ========================================================================================*/
// First-Come, First-Served (FCFS) Scheduling
void fcfs()
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    

    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

        current_time = processes[i].completion_time;
    }




    printf("***************************************************************************************\n\n");
    printf("FCFS Statistics...\n");
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}

/* ========================================================================================*/
// Shortest Job First (SJF) - Non-Preemptive
void sjf_non_preemptive()
{
    // Reset process states
    reset_process_states();

    // Implementation of rest of your code...
    while (completed_processes < num_processes) {
        int minIndex = -1;
        int minBurst = 100000; 

    for (int i = 0; i < num_processes; i++) {
        if (!processes[i].is_completed && processes[i].arrival_time <= current_time) {
            if (processes[i].burst_time < minBurst) {
                minBurst = processes[i].burst_time;
                minIndex = i;
            } else if (processes[i].burst_time == minBurst) {
                // If burst times are equal, select the process that arrived first
                if (processes[i].arrival_time < processes[minIndex].arrival_time) {
                    minIndex = i;
                }
            }
        }
    }

    // If no process is found, advance time
        if (minIndex == -1) {
            current_time++;
        } else {
            // Execute the selected process
            processes[minIndex].completion_time = current_time + processes[minIndex].burst_time;
            processes[minIndex].turnaround_time = processes[minIndex].completion_time - processes[minIndex].arrival_time;
            processes[minIndex].waiting_time = processes[minIndex].turnaround_time - processes[minIndex].burst_time;
            processes[minIndex].is_completed = true;
            
            current_time = processes[minIndex].completion_time; // Move time forward
            completed_processes++;
        }
    }

    // Display results
    printf("***************************************************************************************\n\n");
    printf("SJF (Non-Preemptive) Statistics...\n");
    display_results();
    printf("\n***************************************************************************************\n\n");
}

/* ========================================================================================*/
// Shortest Remaining Time (SRT) - SJF Preemptive
void srt_preemptive()
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    int shortest = -1, min_remaining_time = INT_MAX;
    bool is_found = false;

    while (completed_processes < num_processes) {
        is_found = false;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && !processes[i].is_completed && processes[i].remaining_time < min_remaining_time) {
                    min_remaining_time = processes[i].remaining_time;
                    shortest = i;
                    is_found = true;
            }
        }
    

        if (!is_found) {
            current_time++; // No process available, move time forward
            continue;
        }

        // Process execution (1 unit of time)
        processes[shortest].remaining_time--;
        min_remaining_time = processes[shortest].remaining_time;

        // If the process completes execution
        if (processes[shortest].remaining_time == 0) {
            processes[shortest].completion_time = current_time + 1; // Since index starts at 0
            processes[shortest].turnaround_time = processes[shortest].completion_time - processes[shortest].arrival_time;
            processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;
            processes[shortest].completion_time = true;
            completed_processes++;
            min_remaining_time = INT_MAX; // Reset minRemaining after completion
        }

        current_time++;
    }



    // Display results
    printf("***************************************************************************************\n\n");
    printf("SRT (Preemptive) Statistics...\n");
    display_results();
    printf("\n***************************************************************************************\n\n");
}

/* ========================================================================================*/
// Round Robin (RR) Scheduling
void round_robin(int time_quantum)
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...
    bool is_done = false;

    while (completed_processes < num_processes) {
        is_done = true;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                is_done = false;

                if (processes[i].remaining_time > time_quantum) {
                    current_time += time_quantum;
                    processes[i].remaining_time -= time_quantum;
                } else {
                    current_time += processes[i].remaining_time;
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    processes[i].remaining_time = 0;
                    completed_processes++;
                }
            }
        }


    }


    // If no process was executed, move time forward to the next arrival
    if (is_done) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].remaining_time > 0) {
                current_time = processes[i].arrival_time;
                break;
            }
        }
    }


    printf("***************************************************************************************\n\n");
    printf("RR Statistics (Time Quantum = %d)...\n", time_quantum);
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}

/* ========================================================================================*/
// Priority Scheduling - Non-Preemptive
void priority_non_preemptive()
{

    // Reset process states before execution
    reset_process_states();

    // Implementation of rest of your code...

    // To sort the processes based on priority and arrival time
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = 0; j < num_processes - i - 1; j++) {
            // Sort by priority first, then by arrival time if priorities are the same
            if ((processes[j].priority > processes[j + 1].priority) ||
                (processes[j].priority == processes[j + 1].priority && processes[j].arrival_time > processes[j + 1].arrival_time)) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

        current_time = processes[i].completion_time;
    }


    // Display results
    printf("***************************************************************************************\n\n");
    printf("Priority (PR) - Nonpreemptive Statistics...\n");
    display_results(); // Display Statistics
    printf("\n***************************************************************************************\n");
}
