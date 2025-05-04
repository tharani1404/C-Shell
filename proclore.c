#include"headers.h"
void proclore(int pid,char*home)
{
    char path[4096];
    char buffer[4096];
    char*status;
    char plus;
    long virtual_size;
    char stat_path[4096];
    char exec_path[4096];
    snprintf(path,sizeof(path),"/proc/%d/status",pid);
    FILE*fp;
    fp=fopen(path,"r");
    if(fp==NULL)
    {
        perror("fopen");
        return;
    }
    while(fgets(buffer,sizeof(buffer),fp))
    {
        if(strncmp(buffer,"State:",6)==0)
        {
            status=buffer+7;
            break;
        }
    }
    int pgid = getpgid(pid);
    if (pgid == -1) 
    {
        perror("getpgid");
        return;
    }
    int fg_pgid = tcgetpgrp(STDIN_FILENO);
    if (fg_pgid == -1) 
    {
        perror("tcgetpgrp");
        return;
    }
    if (pid == tcgetpgrp(STDIN_FILENO)) 
    {
        plus = '+';
    } 
    else 
    {
        plus = '\0';
    }
    snprintf(stat_path,sizeof(stat_path),"/proc/%d/stat",pid);
    FILE*stat_file;
    stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) 
    {
        perror("fopen");
        return ;
    }
    snprintf(exec_path, sizeof(exec_path), "/proc/%d/exe", pid);
    readlink(exec_path, exec_path, sizeof(exec_path) - 1);
    if (strlen(exec_path) >= strlen(home))
    {
        snprintf(exec_path, sizeof(exec_path), "~%s", exec_path + strlen(home));
    }
    for (int i = 0; i < 22; i++)
    {
        fscanf(stat_file, "%*s");
    }
    fscanf(stat_file, "%lu", &virtual_size);
    fclose(stat_file);
    printf("pid : %d\n", pid);
    printf("Process status : %c%c\n", status[0], plus);
    printf("Process Group : %d\n", getpgid(pid));
    printf("Virtual memory : %ld\n", virtual_size);
    printf("executable path : %s\n", exec_path);
}
