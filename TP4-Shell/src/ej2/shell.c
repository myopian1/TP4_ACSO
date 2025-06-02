#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';         // reemplazar \n por \0

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        command_count = 0;
        while (token != NULL) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }


        /* You should start programming from here... */
        int pipefds[command_count - 1][2];                  //inicializar pipes
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipefds[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        for (int i = 0; i < command_count; i++) {
            int f=fork();
            if (f < 0) {
                perror("Fork failed");
                exit(1);
            }
            else if (f == 0) { //child
                if (i > 0) {//redirect stdin
                    dup2(pipefds[i-1][0], 0); // Redirect stdin to pipe
                }
                if (i < command_count - 1) { //redirect stdout
                    dup2(pipefds[i][1], 1); 
                }

                for (int j = 0; j < command_count - 1; j++) {
                    close(pipefds[j][0]);
                    close(pipefds[j][1]);
                }

                char *args[32];
                int arg_count = 0;

                // tokenizar
                char *arg_token = strtok(commands[i], " ");
                while (arg_token != NULL && arg_count < 32) {
                    args[arg_count] = arg_token;
                    arg_count++;
                    arg_token = strtok(NULL, " ");
                }
                args[arg_count] = NULL; //terminar argumentos con null

                execvp(args[0], args);
                perror("execvp failed");
                exit(1);
            }
        }
        for (int i = 0; i < command_count - 1; i++) {
            close(pipefds[i][0]);
            close(pipefds[i][1]);
        }
        for (int i = 0; i < command_count; i++) {
            wait(NULL);
        }        
    }
    return 0;
}
