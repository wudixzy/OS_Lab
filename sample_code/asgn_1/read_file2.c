#include <stdio.h>
#include <unistd.h>


int main()
{
    int pid1=-1, pid2=-1, pid3=-1, pid4=-1;
    char str1[100], str2[100], str3[100], str4[100];
    FILE *my_file;
    my_file = fopen("my_file.txt","r");

    pid1 = fork();
    if(pid1 == 0)
    {
        fgets(str1, 100, my_file);
        printf("%s\n",str1);
    }
    if(pid1 != 0 && pid1!= -1)
    {
        pid2 = fork();
        wait();
    }
    if(pid2 == 0)
    {
        fgets(str2, 100, my_file);
        printf("%s\n",str2);
    }
    if(pid1 == 0 || pid2 == 0)
    {
        if(pid3 == -1)pid3=fork();
        else pid4 = fork();
        wait();
    }
    if(pid3 == 0)
    {
        fgets(str3, 100, my_file);
        printf("%s\n",str3);
    }
    else if(pid4 == 0)
    {
        fgets(str4, 100, my_file);
        printf("%s\n",str4);
    }    
}