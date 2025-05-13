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
        " _____  _____ __  __  ___  ____  _   _ _     __  _____ \n"
        "|  _  |/ ____|  \\/  |/ _ \\|  _ \\| | | | |   /_ |/ ___ |\n"
        "| | | | (___ | \\  / | | | | | | | | | | |    | | |  | |\n"
        "| | | |\\___ \\| |\\/| | | | | | | | | | | |    | | |  | |\n"
        "\\ \\_/ /____) | |  | | |_| | |/ /| |_| | |____| | |__| |\n"
        " \\___/|_____/|_|  |_|\\___/|___/  \\___/|______|_|\\_____|\n"
        "\n"
        "Copyright @ R. Aisha Syauqi Ramadhani - 2306250554\n"
        "\n"
        "Type \"help\" to see the list of commands\n"
        "Type \"exit\" to quit the shell\n"
        "Type \"clear\" to clear the screen\n"
        "Type \"{Command} -h\" to see the help for a command\n"
        "Type \"Command1 | Command2\" to pipe output from Command1 to Command2\n"
        "Type \"{Command} | print\" to see the formatted output\n"
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
        
        // Salin input asli untuk digunakan
        char input_copy[1024];
        strcpy(input_copy, input);
        
        // Memeriksa apakah ada pipe dalam perintah
        if (strchr(input, '|') != NULL) {
            // Split command by pipe
            char *cmd1 = strtok(input_copy, "|");
            char *cmd2 = strtok(NULL, "\n");

            if (cmd1 == NULL || cmd2 == NULL) {
                printf("Invalid pipe command\n");
                return;
            }

            // Trim whitespace dari kedua command
            while (*cmd1 == ' ') cmd1++;
            while (*cmd2 == ' ') cmd2++;
            
            // Hapus whitespace di akhir cmd1
            char *end = cmd1 + strlen(cmd1) - 1;
            while (end > cmd1 && *end == ' ') end--;
            *(end + 1) = '\0';

            int pipefd[2];
            pid_t pid1, pid2;

            if (pipe(pipefd) == -1) {
                perror("pipe");
                return;
            }

            pid1 = fork();
            if (pid1 == 0) {
                // CHILD 1 (left command)
                dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout ke pipe
                close(pipefd[0]); // Tutup read end yang tidak digunakan
                close(pipefd[1]); // Tutup setelah dup2

                // Parsing command pertama
                char cmd1_cpy[512];
                strcpy(cmd1_cpy, cmd1);
                
                char *args1[20] = {NULL};
                int i = 0;
                
                char *token = strtok(cmd1_cpy, " \t");
                if (token == NULL) {
                    fprintf(stderr, "Invalid command\n");
                    exit(1);
                }
                
                // Penanganan khusus untuk perintah bawaan
                if (strcmp(token, "itungwoi") == 0) {
                    args1[0] = "./itungwoi";
                } else if (strcmp(token, "print") == 0) {
                    args1[0] = "./print";
                } else if (strcmp(token, "buatdong") == 0) {
                    args1[0] = "./buatdong";
                } else if (strcmp(token, "bacadong") == 0) {
                    args1[0] = "./bacadong";
                } else if (strcmp(token, "rahasiabanget") == 0) {
                    args1[0] = "./rahasiabanget";
                } else if (strcmp(token, "pembacapikiran") == 0) {
                    args1[0] = "./pembacapikiran";
                } else if (strcmp(token, "waktusekarang") == 0) {
                    args1[0] = "./waktusekarang";
                } else if (strcmp(token, "hitungram") == 0) {
                    args1[0] = "./hitungram";
                } else if (strcmp(token, "rename") == 0) {
                    args1[0] = "./rename";
                } else if (strcmp(token, "hapusfile") == 0) {
                    args1[0] = "./hapusfile";
                } else if (strcmp(token, "list") == 0) {
                    args1[0] = "/bin/ls";
                } else if (strcmp(token, "clear") == 0) {
                    args1[0] = "/bin/clear";
                } else if (strcmp(token, "caridong") == 0) {
                    args1[0] = "./caridong";
                } else if (strcmp(token, "sortirdong") == 0) {
                    args1[0] = "./sortirdong";
                } else if (strcmp(token, "proseswoi") == 0) {
                    args1[0] = "./proseswoi";
                } else if (strcmp(token, "netwoi") == 0) {
                    args1[0] = "./netwoi";
                } else if (strcmp(token, "statsdong") == 0) {
                    args1[0] = "./statsdong";
                } else {
                    args1[0] = token;  // perintah yang tidak dikenali
                }
                
                i = 1;
                while ((token = strtok(NULL, " \t")) != NULL && i < 19) {
                    args1[i++] = token;
                }
                args1[i] = NULL;
                
                execvp(args1[0], args1);
                perror("exec failed");
                exit(1);
            }

            pid2 = fork();
            if (pid2 == 0) {
                // CHILD 2 (right command)
                dup2(pipefd[0], STDIN_FILENO); // Redirect stdin dari pipe
                close(pipefd[1]); // Tutup write end yang tidak digunakan
                close(pipefd[0]); // Tutup setelah dup2

                // Parsing command kedua
                char cmd2_cpy[512];
                strcpy(cmd2_cpy, cmd2);
                
                char *args2[20] = {NULL};
                int j = 0;
                
                char *token = strtok(cmd2_cpy, " \t");
                if (token == NULL) {
                    fprintf(stderr, "Invalid command\n");
                    exit(1);
                }
                
                // Penanganan khusus untuk perintah bawaan
                if (strcmp(token, "itungwoi") == 0) {
                    args2[0] = "./itungwoi";
                } else if (strcmp(token, "print") == 0) {
                    args2[0] = "./print";
                } else if (strcmp(token, "buatdong") == 0) {
                    args2[0] = "./buatdong";
                } else if (strcmp(token, "bacadong") == 0) {
                    args2[0] = "./bacadong";
                } else if (strcmp(token, "rahasiabanget") == 0) {
                    args2[0] = "./rahasiabanget";
                } else if (strcmp(token, "pembacapikiran") == 0) {
                    args2[0] = "./pembacapikiran";
                } else if (strcmp(token, "waktusekarang") == 0) {
                    args2[0] = "./waktusekarang";
                } else if (strcmp(token, "hitungram") == 0) {
                    args2[0] = "./hitungram";
                } else if (strcmp(token, "rename") == 0) {
                    args2[0] = "./rename";
                } else if (strcmp(token, "hapusfile") == 0) {
                    args2[0] = "./hapusfile";
                } else if (strcmp(token, "list") == 0) {
                    args2[0] = "/bin/ls";
                } else if (strcmp(token, "clear") == 0) {
                    args2[0] = "/bin/clear";
                } else if (strcmp(token, "caridong") == 0) {
                    args2[0] = "./caridong";
                } else if (strcmp(token, "sortirdong") == 0) {
                    args2[0] = "./sortirdong";
                } else if (strcmp(token, "proseswoi") == 0) {
                    args2[0] = "./proseswoi";
                } else if (strcmp(token, "netwoi") == 0) {
                    args2[0] = "./netwoi";
                } else if (strcmp(token, "statsdong") == 0) {
                    args2[0] = "./statsdong";
                } else {
                    args2[0] = token;  // perintah yang tidak dikenali
                }
                
                j = 1;
                while ((token = strtok(NULL, " \t")) != NULL && j < 19) {
                    args2[j++] = token;
                }
                args2[j] = NULL;
                
                execvp(args2[0], args2);
                perror("exec failed");
                exit(1);
            }

            // Parent process
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            return;
        }
        
        // Kode untuk menangani perintah tanpa pipe
        char *token = strtok(input, " "); // Mulai parsing command normal
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
        else if (strcmp(token, "caridong") == 0)
    {
        char *args[20] = {NULL};
        args[0] = "./caridong";
        
        int i = 1;
        char *arg;
        while ((arg = strtok(NULL, " ")) != NULL && i < 19) {
            args[i++] = arg;
        }
        args[i] = NULL;
        
        pid_t pid = fork();
        if (pid == 0) {
            execv("./caridong", args);
            perror("exec failed");
            exit(1);
        } else {
            wait(NULL);
        }
        }
        else if (strcmp(token, "sortirdong") == 0)
        {
            char *args[20] = {NULL};
            args[0] = "./sortirdong";
            
            int i = 1;
            char *arg;
            while ((arg = strtok(NULL, " ")) != NULL && i < 19) {
                args[i++] = arg;
            }
            args[i] = NULL;
            
            pid_t pid = fork();
            if (pid == 0) {
                execv("./sortirdong", args);
                perror("exec failed");
                exit(1);
            } else {
                wait(NULL);
            }
        }
        else if (strcmp(token, "proseswoi") == 0)
        {
            char *args[20] = {NULL};
            args[0] = "./proseswoi";
            
            int i = 1;
            char *arg;
            while ((arg = strtok(NULL, " ")) != NULL && i < 19) {
                args[i++] = arg;
            }
            args[i] = NULL;
            
            pid_t pid = fork();
            if (pid == 0) {
                execv("./proseswoi", args);
                perror("exec failed");
                exit(1);
            } else {
                wait(NULL);
            }
        }
        else if (strcmp(token, "netwoi") == 0)
        {
            char *args[20] = {NULL};
            args[0] = "./netwoi";
            
            int i = 1;
            char *arg;
            while ((arg = strtok(NULL, " ")) != NULL && i < 19) {
                args[i++] = arg;
            }
            args[i] = NULL;
            
            pid_t pid = fork();
            if (pid == 0) {
                execv("./netwoi", args);
                perror("exec failed");
                exit(1);
            } else {
                wait(NULL);
            }
        }
        else if (strcmp(token, "statsdong") == 0)
        {
            char *args[20] = {NULL};
            args[0] = "./statsdong";
            
            int i = 1;
            char *arg;
            while ((arg = strtok(NULL, " ")) != NULL && i < 19) {
                args[i++] = arg;
            }
            args[i] = NULL;
            
            pid_t pid = fork();
            if (pid == 0) {
                execv("./statsdong", args);
                perror("exec failed");
                exit(1);
            } else {
                wait(NULL);
            }
        }
    }
}