#include"headers.h"
void trim_command(char *command) 
{
    char *start = command;
    while (*start == ' ' || *start == '\t') start++;
    memmove(command, start, strlen(start) + 1);
    char *end = command + strlen(command) - 1;
    while (end > command && (*end == ' ' || *end == '\t')) end--;
    *(end + 1) = '\0';
}
void clear_fg_commands(FG* fg_commands, int* fg_count) 
{
    for (int i = 0; i < *fg_count; ++i) 
    {
        free(fg_commands[i].command); 
    }
    *fg_count = 0; 
}
void execute_background_command(char* background_command) 
{
    trim_command(background_command);
    if (strlen(background_command) == 0) 
    {
        printf("\033[31mERROR:\033[0m Invalid Command: Command is empty\n");
        return;
    }
    int pid = fork();
    if (pid < 0) 
    {
        printf("\033[31mERROR:\033[0m Fork Failed\n");
        exit(1);
    } 
    else if (pid == 0) 
    { 
        char* args[100];
        int i = 0;
        char* command_copy = strdup(background_command);
        if (command_copy == NULL) 
        {
            perror("strdup");
            exit(1);
        }
        args[i] = strtok(command_copy, " ");
        while (args[i] != NULL) 
        {
            args[++i] = strtok(NULL, " ");
        }
        if (execvp(args[0], args) < 0) 
        {
            fprintf(stderr, "\033[31mERROR:\033[0m Invalid Command: %s\n", args[0]);
            fflush(stderr);  
            free(command_copy);
            exit(1);
        }
        free(command_copy);
        exit(1);
    } 
    else 
    { 
        add_process_list(pid, background_command);
        printf("%d\n", pid);
    }
}
void execute_foreground_command(char* foreground_command, commandlog *log, char* home, char* prev_direc,FG*fg_commands,int*fg_count) 
{
    if (strncmp(foreground_command, "hop", 3) == 0) 
    {
        char* path = foreground_command + 4;  
        if (*path == '\0')
        {
            char current_dir[4096];
            if (getcwd(current_dir, sizeof(current_dir)) == NULL) 
            {
                perror("getcwd");
                return;
            }
            char dir[4096];
            strcpy(dir, home);
            if (chdir(dir) != 0) 
            {
                perror("chdir");
                return;
            }
            strcpy(prev_direc, current_dir);
            printf("%s\n",dir);
            return;
        }
        trim_command(path); 
        char*new_paths[4096];
        int i=0;
        int count=0;
        path=strtok(path," ");
        
        while(path!=NULL)
        {
            new_paths[i++]=path;
            count++;
            path=strtok(NULL," ");
        }
        for(int j=0;j<count;j++)
        {
            change_directory(new_paths[j], home, prev_direc);
        }
        return;
    }
    else if (strncmp(foreground_command, "reveal", 6) == 0) 
    {
        char* flags = foreground_command + 7;  
        int p = 0;
        int n = strlen(flags);
        if (flags[n - 1] == '-') 
        {
            p = 1;
        }
        if(p==1 && strlen(prev_direc)==0)
        {
            printf("\033[31mERROR:\033[0m No previous directory.\n");
            return;
        }
        flags=strtok(flags," ");
        int l = 0;
        int a = 0;
        char new_dir[4096];
        if (getcwd(new_dir, sizeof(new_dir)) == NULL)
        {
            perror("getcwd");
            return ;
        }
        char*new_flags[4096];
        int i=0;
        int count=0;
        while (flags!=NULL)
        {
            new_flags[i++]=flags;
            count++;
        
        flags=strtok(NULL," ");
        }
        for(int j=0;j<count;j++)
        {
            set_flags(new_flags[j], home, &l, &a, new_dir, prev_direc, p,sizeof(new_dir));
        }
        reveal_files(new_dir, l, a);
        return;
    }
    else if(strncmp(foreground_command,"proclore",8)==0)
    {
        int pid;
        char*x=foreground_command+9;
        if(*x=='\0')
        {
             pid=getpid();
        }
        else
        {
            pid=atoi(x);  
        }
        proclore(pid,home);
        return;
    }
    else if(strncmp(foreground_command,"seek",4)==0)
    {
        char*seek_command=foreground_command+5;
        seek_command=strtok(seek_command," ");
        int d = 0;
        int f = 0;
        int e=0;
        int w=0;
        char name[4096];
        char dir[4096];
        int k=sizeof(dir);
        int file_count=0;
        int dir_count=0;
        if (getcwd(dir, k) == NULL)
        {
            perror("getcwd");
            return ;
        }
        int n=strlen(seek_command);
            int p=0;
            if( (n-1>=0) && (seek_command[n-1]=='-'))
            {
                p=1;
            }
            char *new_seek_commands[4096];
            int i=0;
            int count=0;
        while(seek_command!=NULL)
        {
            new_seek_commands[i++]=seek_command;
            count++;
            seek_command=strtok(NULL," ");
        }
        for(int j=0;j<count;j++)
        {
        set_flags_for_seek(new_seek_commands[j],&d,&f,&e,name,dir,home,prev_direc,p,sizeof(dir),&w);
        }
        if(f==1 && d==1)
        {
            printf("\033[31mERROR:\033[0m INVALID FLAGS\n");
            return;
        }
        char ret_direc[4096];
        int m=sizeof(ret_direc);
        seek_file(d,f,e,name,dir,dir,&file_count,&dir_count,ret_direc);
        if(file_count+dir_count==0)
        {
            printf("NO MATCH FOUND\n");
            return;
        }
        handle_e_flag(e,file_count,dir_count,dir,ret_direc,m);
        return;
    }
    else if (strcmp(foreground_command, "log") == 0) 
    {
        print_log(log);
        return;
    }
    else if (strcmp(foreground_command, "log purge") == 0) 
    {
        log_purge(log);
        save_log(log, "log.txt",home);
        return;
    } 
    else if (strncmp(foreground_command, "log execute ", 12) == 0)
    {
        int n = atoi(foreground_command + 12);
        execute_command(log, n, home, prev_direc,fg_commands,fg_count);
        save_log(log, "log.txt",home);
        return;
    }

    time_t start_time = time(NULL);
    int pid = fork();
    if (pid < 0) 
    {
        perror("fork");
        exit(1);
    } 
    else if (pid == 0) 
    {
        char *args[100];
        int i = 0;
        args[i] = strtok(foreground_command, " ");
        while (args[i] != NULL) 
        {
            args[++i] = strtok(NULL, " ");
        }
        if (execvp(args[0], args) < 0) 
        {
            printf("\033[31mERROR:\033[0m Invalid Command\n");
            exit(1);
        }
    } 
    else 
    {
        int status;
        waitpid(pid, &status, 0);

        time_t end_time = time(NULL);


        int elapsed_time = (int) round(end_time - start_time);


        if (elapsed_time > 2) 
        {
            fg_commands[*fg_count].command = malloc(strlen(foreground_command) + 1);
            if (fg_commands[*fg_count].command == NULL) 
            {
                perror("malloc");
                return;
            }
            strcpy(fg_commands[*fg_count].command, foreground_command);
            fg_commands[*fg_count].time = elapsed_time;
            (*fg_count)++;
        }
    }
}

void handle_sigchld(int sig) 
{
    int saved_errno = errno;  
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 
    {
        int found = 0;
        for (int i = 0; i < *global_process_count; i++) 
        {
            if (global_process_list[i].pid == pid) 
            {
                printf("%s with pid %d ", global_process_list[i].name,pid);
                if (WIFEXITED(status)) 
                {
                    printf("exited normally with status %d\n", WEXITSTATUS(status));
                } 
                else if (WIFSIGNALED(status)) 
                {
                    printf("was terminated by signal %d\n", WTERMSIG(status));
                } 
                else 
                {
                    printf("terminated abnormally\n");
                }
                found = 1;
                for (int j = i; j < *global_process_count - 1; j++) 
                {
                    global_process_list[j] = global_process_list[j + 1];
                }
                (*global_process_count)--;
                break;
            }
        }
        if (!found) 
        {
            printf("Process with PID %d not found in the process list\n", pid);
        }
    }
    errno = saved_errno;  
}
void add_process_list(int pid, char* background_command) 
{
    global_process_list[*global_process_count].pid = pid;
    strcpy(global_process_list[*global_process_count].name, background_command);
    (*global_process_count)++;
}

