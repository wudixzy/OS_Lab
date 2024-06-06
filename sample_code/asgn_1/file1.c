#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <x> <n>\n", argv[0]);
        return 1;
    }

    double x = atof(argv[1]);
    int n = atoi(argv[2]);

    if (x >= 1 || n < 1) {
        fprintf(stderr, "Invalid input: x should be less than 1 and n should be 1 or more.\n");
        return 1;
    }

    int pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process
        char x_str[20], n_str[20];
        snprintf(x_str, sizeof(x_str), "%f", x);
        snprintf(n_str, sizeof(n_str), "%d", n);
        execl("./file2", "file2", x_str, n_str, (char *) NULL);
        perror("execl");
        return 1;
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("Parent(PID=%d): Done\n", getpid());
    }

    return 0;
}
