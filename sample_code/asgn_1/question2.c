#include <stdio.h>
#include <unistd.h>

int main()
{
    int pid;
    pid = fork();
    if(pid != 0)
    {
        wait();
        printf("pid=%d\n", pid);
    }
    return 0;


}