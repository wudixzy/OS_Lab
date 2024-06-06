#include <stdio.h>
#include <stdlib.h>

double calculate_ex(double x, int n) {
    double result = 1.0;
    double term = 1.0;

    for (int i = 1; i < n; ++i) {
        term *= x / i;
        result += term;
    }

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <x> <n>\n", argv[0]);
        return 1;
    }

    double x = atof(argv[1]);
    int n = atoi(argv[2]);

    double result = calculate_ex(x, n);
    printf("Child(PID=%d): For x = %f the first %d terms yields %f\n", getpid(), x, n, result);

    return 0;
}
