#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

// Function Prototypes
char *takeInput();
void parseInput(char *input);
void landingPage();

// SIGNAL HANDLING (diletakkan DI LUAR main)
void handle_sigint(int sig) {
    printf("\nNggak bisa keluar pake Ctrl+C ya! Ketik 'exit' dong.\nnetlab>> ");
    fflush(stdout);
}

void handle_sigtstp(int sig) {
    printf("\nShell ini nggak bisa di-pause! 😎\nnetlab>> ");
    fflush(stdout);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    landingPage(); 

    while (1) {
        char *input = takeInput();
        parseInput(input); 
        free(input);  
    }

    return 0;
}

void landingPage()
{
    puts(
        " _____ _____  ___  ______________ _   _ _         _____ \n"
        "|  _  /  ___| |  \\/  |  _  |  _  \\ | | | |       |  _  |\n"
        "| | | \\ `--.  | .  . | | | | | | | | | | |       | |_| |\n"
        "| | | |`--. \\ | |\\/| | | | | | | | | | | |       \\____ |\n"
        "\\ \\_/ /\\__/ / | |  | \\ \\_/ / |/ /| |_| | |____  .___/ /\n"
        " \\___/\\____/  \\_|  |_/\\___/|___/  \\___/\\_____/  \\____/ \n"
        "\n"
        "Copyright @ R. Aisha Syauqi Ramadhani - 2306250554\n"
        "Type \"help\" to see the list of commands\n"
        "Type \"exit\" to quit the shell\n"
        "Type \"clear\" to clear the screen\n"
        "Type \"{Command} -h\" to see the help for a command\n"
    );
}


char *takeInput()
{
    char *input;
    while (1)
    {
        input = readline("netlab>> ");
        if (strlen(input) != 0)
            return input;  
    }
}

void parseInput(char *input)
{
    if (strcmp(input, "exit") == 0)
        exit(0);  
    else if (strlen(input) != 0)
    {
        add_history(input);  
        char *token = strtok(input, " "); 
if (strchr(input, '|') != NULL) {
    // Split command by pipe
    char *cmd1 = strtok(input, "|");
    char *cmd2 = strtok(NULL, "\n");

    if (cmd1 == NULL || cmd2 == NULL) {
        printf("Invalid pipe command\n");
        return;
    }

    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid1 = fork();
    if (pid1 == 0) {
        // CHILD 1 (left command)
        dup2(pipefd[1], STDOUT_FILENO); // write to pipe
        close(pipefd[0]); // close unused read end
        close(pipefd[1]);

        char *args1[10];
        int i = 0;
        char *token = strtok(cmd1, " ");
        while (token != NULL && i < 9) {
            args1[i++] = token;
            token = strtok(NULL, " ");
        }
        args1[i] = NULL;
        execvp(args1[0], args1);
        perror("exec1 failed");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // CHILD 2 (right command)
        dup2(pipefd[0], STDIN_FILENO); // read from pipe
        close(pipefd[1]); // close unused write end
        close(pipefd[0]);

        char *args2[10];
        int j = 0;
        char *token2 = strtok(cmd2, " ");
        while (token2 != NULL && j < 9) {
            args2[j++] = token2;
            token2 = strtok(NULL, " ");
        }
        args2[j] = NULL;
        execvp(args2[0], args2);
        perror("exec2 failed");
        exit(1);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
    return;
}

        if (token == NULL)
        {
            return; 
        }
        else if (strcmp(token, "help") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execl("./help", "help", NULL); 
                perror("exec failed");
                exit(1);
            }
            else
            {
                wait(NULL);  
            }
        }
        else if (strcmp(token, "print") == 0) {
        char *args[10];
        args[0] = "./print"; 

        int i = 1;
        char *arg;
        while ((arg = strtok(NULL, " ")) != NULL && i < 9) {
            args[i++] = arg;
        }
        args[i] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            execv("./print", args);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
        }
    }   
           else if (strcmp(token, "buatdong") == 0)
{
    char *arg1 = strtok(NULL, " ");  // Bisa filename atau flag
    char *arg2 = strtok(NULL, "\n"); // Bisa content atau NULL

    if (arg1 != NULL && strcmp(arg1, "-h") == 0) {
        // Kirim flag -h saja
        pid_t pid = fork();
        if (pid == 0) {
            execl("./buatdong", "buatdong", "-h", NULL);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
        }
    }
    else if (arg1 != NULL && strcmp(arg1, "--help") == 0) {
        // Kirim flag --help saja
        pid_t pid = fork();
        if (pid == 0) {
            execl("./buatdong", "buatdong", "--help", NULL);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
        }
    }
    else if (arg1 != NULL && arg2 != NULL) {
        // Normal case: filename + content
        pid_t pid = fork();
        if (pid == 0) {
            execl("./buatdong", "buatdong", arg1, arg2, NULL);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
        }
    }
    else {
        printf("Usage: buatdong {filename} {content}\n");
    }
}

        else if (strcmp(token, "bacadong") == 0)
{
    char *arg = strtok(NULL, "\n");  // Bisa filename atau flag

    if (arg != NULL)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            execl("./bacadong", "bacadong", arg, NULL);
            perror("exec failed");
            exit(1);
        }
        else
        {
            wait(NULL);
        }
    }
    else
    {
        printf("Usage: bacadong {filename}\n");
    }
}

       else if (strcmp(token, "rahasiabanget") == 0)
{
    char *arg1 = strtok(NULL, " ");  // Bisa -h atau filename
    char *arg2 = strtok(NULL, "\n"); // Isi content kalau ada

    pid_t pid = fork();
    if (pid == 0)
    {
        if (arg1 != NULL && (strcmp(arg1, "-h") == 0 || strcmp(arg1, "--help") == 0)) {
            execl("./rahasiabanget", "rahasiabanget", "-h", NULL);
        } else if (arg1 != NULL && arg2 != NULL) {
            execl("./rahasiabanget", "rahasiabanget", arg1, arg2, NULL);
        } else if (arg1 != NULL) {
            execl("./rahasiabanget", "rahasiabanget", arg1, NULL);
        } else {
            printf("Usage: rahasiabanget {filename} {content}\n");
        }
        perror("exec failed");
        exit(1);
    }
    else
    {
        wait(NULL);
    }
}  
       else if (strcmp(token, "pembacapikiran") == 0) {
    char *arg = strtok(NULL, "\n");  // filename atau -h

    pid_t pid = fork();
    if (pid == 0) {
        if (arg != NULL) {
            execl("./pembacapikiran", "pembacapikiran", arg, NULL);
        } else {
            execl("./pembacapikiran", "pembacapikiran", NULL);
        }
        perror("exec failed");
        exit(1);
    } else {
        wait(NULL);
    }
}

   else if (strcmp(token, "itungwoi") == 0)
        {
            char *operation = strtok(NULL, " ");
            char *num1_str = strtok(NULL, " ");
            char *num2_str = strtok(NULL, " ");
           
            pid_t pid = fork();
            if (pid == 0)
            {
                if (operation != NULL && num1_str != NULL && num2_str != NULL) {
                    execl("./itungwoi", "itungwoi", operation, num1_str, num2_str, NULL);
                } else if (operation != NULL) {
                    execl("./itungwoi", "itungwoi", operation, NULL);
                } else {
                    execl("./itungwoi", "itungwoi", NULL);
                }
                perror("exec failed");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }


        else if (strcmp(token, "clear") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execl("/bin/clear", "clear", NULL);  
                perror("exec failed");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else if (strcmp(token, "waktusekarang") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execl("./waktusekarang", "waktusekarang", NULL);  
                perror("exec failed");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else if (strcmp(token, "hitungram") == 0)
        {
            char *filename = strtok(NULL, " ");
            if (filename != NULL)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    execl("./hitungram", "hitungram", filename, NULL); 
                    perror("exec failed");
                    exit(1);
                }
                wait(NULL);
            }
            else
            {
                printf("Usage: hitungram {filename}\n");
            }
        }
        else if (strcmp(token, "list") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                execl("/bin/ls", "ls", "-l", NULL); 
                perror("exec failed");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }
        else if (strcmp(token, "rename") == 0)
        {
            char *oldname = strtok(NULL, " ");
            char *newname = strtok(NULL, " ");
            if (oldname != NULL && newname != NULL)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    execl("./rename", "rename", oldname, newname, NULL);  
                    perror("exec failed");
                    exit(1);
                }
                wait(NULL);
            }
            else
            {
                printf("Usage: rename {oldname} {newname}\n");
            }
        }

        else if (strcmp(token, "hapusfile") == 0)  
        {
            char *filename = strtok(NULL, " ");  

            if (filename != NULL)
            {
                pid_t pid = fork();  
                if (pid == 0)
                {
                    execl("./hapusfile", "hapusfile", filename, NULL); 
                    perror("exec failed");
                    exit(1);
                }
                wait(NULL);  
            }
            else
            {
                printf("Usage: hapusfile {filename}\n");  
            }
        }
    }
}