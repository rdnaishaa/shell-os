#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <pwd.h>

#define MAX_PROCESSES 1000
#define PATH_MAX 4096

typedef struct {
    int pid;
    char user[256];
    float cpu_usage;
    float memory_usage;
    char command[256];
} Process;

Process processes[MAX_PROCESSES];
int process_count = 0;

void print_help() {
    printf("\n===== PROSESWOI HELP =====\n");
    printf("Usage: proseswoi\n");
    printf("Description: List or manage running processes\n\n");
    printf("Flags:\n");
    printf("  -a: Show all processes\n");
    printf("  -k {pid}: Kill process by PID\n");
    printf("  -m: Sort by memory usage\n");
    printf("  -c: Sort by CPU usage\n");
    printf("  -h, --help: Show this help message\n\n");
    printf("Examples:\n");
    printf("  proseswoi: Show your processes\n");
    printf("  proseswoi -a: Show all processes\n");
    printf("  proseswoi -k 1234: Kill process with PID 1234\n");
    printf("  proseswoi -am: Show all processes sorted by memory usage\n");
}

// Get processes from /proc
void get_processes(int show_all) {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("Cannot open /proc");
        return;
    }
    
    uid_t my_uid = getuid();
    struct dirent *entry;
    
    while ((entry = readdir(proc_dir)) != NULL && process_count < MAX_PROCESSES) {
        // Check if directory name is a number (PID)
        if (entry->d_type == DT_DIR) {
            char *end;
            int pid = strtol(entry->d_name, &end, 10);
            if (*end != '\0') {
                continue;  // Not a number
            }
            
            // Construct path to status file
            char status_path[PATH_MAX];
            snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
            
            FILE *status_file = fopen(status_path, "r");
            if (!status_file) {
                continue;  // Can't open status file
            }
            
            // Get user ID, memory usage, and command
            uid_t uid = 0;
            char line[256];
            unsigned long vm_size = 0;
            
            while (fgets(line, sizeof(line), status_file)) {
                if (strncmp(line, "Uid:", 4) == 0) {
                    sscanf(line, "Uid: %d", &uid);
                }
                if (strncmp(line, "VmSize:", 7) == 0) {
                    sscanf(line, "VmSize: %lu", &vm_size);
                }
            }
            fclose(status_file);
            
            // Skip if not showing all and not user's process
            if (!show_all && uid != my_uid) {
                continue;
            }
            
            // Get command name
            char cmdline_path[PATH_MAX];
            snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);
            
            FILE *cmdline_file = fopen(cmdline_path, "r");
            if (cmdline_file) {
                fgets(processes[process_count].command, sizeof(processes[process_count].command), cmdline_file);
                fclose(cmdline_file);
            }
            
            if (strlen(processes[process_count].command) == 0) {
                // If cmdline is empty, try to get command from comm
                char comm_path[PATH_MAX];
                snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm", pid);
                
                FILE *comm_file = fopen(comm_path, "r");
                if (comm_file) {
                    fgets(processes[process_count].command, sizeof(processes[process_count].command), comm_file);
                    // Remove newline
                    size_t len = strlen(processes[process_count].command);
                    if (len > 0 && processes[process_count].command[len - 1] == '\n') {
                        processes[process_count].command[len - 1] = '\0';
                    }
                    fclose(comm_file);
                }
            }
            
            // Get username from UID
            struct passwd *pwd = getpwuid(uid);
            if (pwd) {
                strncpy(processes[process_count].user, pwd->pw_name, sizeof(processes[process_count].user));
            } else {
                snprintf(processes[process_count].user, sizeof(processes[process_count].user), "%d", uid);
            }
            
            processes[process_count].pid = pid;
            processes[process_count].memory_usage = (float)vm_size / 1024.0;  // Convert to MB
            
            // Get CPU usage (simplified)
            char stat_path[PATH_MAX];
            snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
            
            FILE *stat_file = fopen(stat_path, "r");
            if (stat_file) {
                unsigned long utime, stime;
                fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu", &utime, &stime);
                processes[process_count].cpu_usage = (float)(utime + stime) / 100.0;
                fclose(stat_file);
            } else {
                processes[process_count].cpu_usage = 0.0;
            }
            
            process_count++;
        }
    }
    
    closedir(proc_dir);
}

// Function to kill a process
int kill_process(int pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("Process %d terminated successfully\n", pid);
        return 0;
    } else {
        perror("Failed to kill process");
        return 1;
    }
}

// Compare function for sorting by memory usage
int compare_memory(const void *a, const void *b) {
    Process *process_a = (Process *)a;
    Process *process_b = (Process *)b;
    return process_b->memory_usage - process_a->memory_usage;
}

// Compare function for sorting by CPU usage
int compare_cpu(const void *a, const void *b) {
    Process *process_a = (Process *)a;
    Process *process_b = (Process *)b;
    return process_b->cpu_usage - process_a->cpu_usage;
}

int main(int argc, char *argv[]) {
    int show_all = 0;
    int sort_by_memory = 0;
    int sort_by_cpu = 0;
    int kill_pid = -1;
    
    // Check for help flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    // Parse flags
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // It's a flag
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'a': show_all = 1; break;
                    case 'm': sort_by_memory = 1; break;
                    case 'c': sort_by_cpu = 1; break;
                    case 'k': 
                        if (i + 1 < argc) {
                            kill_pid = atoi(argv[i + 1]);
                            if (kill_pid <= 0) {
                                fprintf(stderr, "Invalid PID: %s\n", argv[i + 1]);
                                return 1;
                            }
                            return kill_process(kill_pid);
                        } else {
                            fprintf(stderr, "No PID provided for kill command\n");
                            return 1;
                        }
                        break;
                    case 'h': print_help(); return 0;
                    default:
                        fprintf(stderr, "Unknown flag: %c\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        }
    }
    
    // Get processes
    get_processes(show_all);
    
    // Sort processes
    if (sort_by_memory) {
        qsort(processes, process_count, sizeof(Process), compare_memory);
    } else if (sort_by_cpu) {
        qsort(processes, process_count, sizeof(Process), compare_cpu);
    }
    
    // Print header
    printf("%-8s %-12s %-8s %-8s %s\n", "PID", "USER", "CPU%", "MEM(MB)", "COMMAND");
    
    // Print processes
    for (int i = 0; i < process_count; i++) {
        printf("%-8d %-12s %-8.1f %-8.1f %s\n", 
               processes[i].pid, 
               processes[i].user, 
               processes[i].cpu_usage, 
               processes[i].memory_usage, 
               processes[i].command);
    }
    
    return 0;
}