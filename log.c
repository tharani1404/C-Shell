#include"headers.h"
void add_command_to_log(commandlog*log,char*command)
{
    if(log->count>0 && strcmp(log->commands[(log->start+log->count-1) % MAX_C],command)==0)
    {
        return;
    }
    strcpy(log->commands[(log->start+log->count) % MAX_C],command);
    if(log->count==MAX_C)
    {
        log->start=(log->start+1)%MAX_C;
    }
    else
    {
        log->count++;
    }
}
void print_log(commandlog*log)
{
    if(log->count==0)
    {
        printf("Log is Empty\n");
    }
    for(int i=0;i<log->count;i++)
    {
        int j=(log->start+i) % MAX_C;
        printf("%s\n",log->commands[j]);
    }
}
void log_purge(commandlog*log)
{
    log->start=0;
    log->count=0;
}
void execute_command(commandlog*log,int n,char*home,char*prev_direc,FG*fg_commands,int*fg_count)
{
    if(n<1 || n>log->count)
    {
        printf("Invalid Command");
        return;
    }
    int m=(log->start+log->count-n) % MAX_C;
    char*new_command=log->commands[m];
    printf("Executing:%s\n",new_command);
    int input_length=strlen(new_command);
    char background_command[4096];
    char foreground_command[4096];
    for (int i = 0; i < input_length;) 
    {
        if (new_command[i] == '&' || new_command[i] == ';') 
        {
            if (new_command[i] == '&') 
            {
                strncpy(background_command, new_command, i);
                background_command[i] = '\0';
                trim_command(background_command);
                if (strlen(background_command) > 0) 
                {
                    execute_background_command(background_command);
                }
                memmove(new_command, new_command + i + 1, input_length - i);
            } 
            else 
            {
                    strncpy(foreground_command, new_command, i);
                    foreground_command[i] = '\0';
                    trim_command(foreground_command);
                    if (strlen(foreground_command) > 0) 
                    {
                        execute_foreground_command(foreground_command, log, home, prev_direc,fg_commands,fg_count);
                    }
                    memmove(new_command, new_command + i + 1, input_length - i);
            }
            input_length = strlen(new_command);  
            i = 0;  
        } 
        else 
        {
            i++;  
        }
    }
    trim_command(new_command);
    if (strlen(new_command) > 0) 
    {
        execute_foreground_command(new_command, log, home, prev_direc,fg_commands,fg_count);
    }
    if(strcmp(new_command,log->commands[(log->start+log->count-1) % MAX_C])!=0)
    {
        add_command_to_log(log,new_command);
    }
}
void load_log(commandlog* log, char* filename) 
{
    FILE* file = fopen(filename, "r");
    if (!file) 
    {
        perror("fopen");
        return;
    }
    
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), file) != NULL) 
    {
        buffer[strcspn(buffer, "\n")] = '\0'; 
        if (strlen(buffer) > 0) 
        { 
            strcpy(log->commands[log->count % MAX_C], buffer);
            if (log->count == MAX_C) 
            {
                log->start = (log->start + 1) % MAX_C;
            } 
            else 
            {
                log->count++;
            }
        }
    }
    fclose(file);
}
void save_log(commandlog* log, char* filename, char* home) 
{
    char filepath[4096];
    snprintf(filepath, sizeof(filepath), "%s/%s", home, filename);
    FILE* file = fopen(filepath, "w");
    if (file == NULL) 
    {
        perror("fopen");
        return;
    }
    for (int i = 0; i < log->count; i++) 
    {
        int index = (log->start + i) % MAX_C;
        fprintf(file, "%s\n", log->commands[index]);
    }
    fclose(file);
}






