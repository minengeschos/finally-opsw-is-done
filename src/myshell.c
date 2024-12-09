#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "ls_command.h"

#define MAX_LINE 80
#define MAX_ARGS 10

int main(void) {
    char input[MAX_LINE];
    char *token;
    char *argv[MAX_ARGS];

    while (1) {
        //prompt
        printf("myshell> ");

        //read command
        fgets(input, MAX_LINE, stdin);

        //tokenize input
        token = strtok(input, " \n\t");
        int i = 0;
        while (token != NULL) {
            argv[i++] = token;
            token = strtok(NULL, " \n\t");
        }
        argv[i] = NULL;

        if (argv[0] == NULL) { //empty command
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            printf("Good bye!\n");
            exit(0);
        } else if (strcmp(argv[0], "cd") == 0) {
            chdir(argv[1]);
        } else if (strcmp(argv[0], "pwd") == 0) {
            getcwd(input, MAX_LINE);
            printf("%s\n", input);
        } else if (strcmp(argv[0], "ls") == 0) {
            my_ls();
        } else if (strcmp(argv[0], "cat") == 0) {
            //cat 구현
            if (argv[1] == NULL) {
                printf("cat: 파일 이름이 필요합니다.\n");
                continue;
            }

            int fd = open(argv[1], O_RDONLY);
            if (fd == -1) {
                perror("cat");
                continue;
            }

            char buffer[256];
            ssize_t bytesRead;
            while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
                write(STDOUT_FILENO, buffer, bytesRead);
            }
            printf("\n");

            if (bytesRead == -1) {
                perror("cat");
            }

            close(fd);
        } else {
            if (access(argv[0], X_OK) == 0) {
                // execute 구현
                pid_t pid = fork();
                if (pid == 0) { // child process
                    execvp(argv[0], argv);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                } else if (pid > 0) { // parent process
                    wait(NULL);
                } else {
                    perror("fork");
                }
            } else {
                printf("command not found: %s\n", argv[0]);
            }
        }
    }
    return 0;
}
