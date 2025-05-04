#include "headers.h"
processlist process_list[256];
int process_count = 0;

processlist *global_process_list = process_list;
int *global_process_count = &process_count;

int main() 
{
    commandlog log;
    log.start = 0;
    log.count = 0;
    load_log(&log, "log1.txt");
    char home[4096];
    char prev_direc[4096] = "";
    char input_command[4096];
    char background_command[4096];
    char foreground_command[4096];
    FG fg_commands[256];
    int fg_count = 0;
    if (getcwd(home, sizeof(home)) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
    while (1) 
    {
        prompt_user(home, fg_commands, fg_count);
        clear_fg_commands(fg_commands, &fg_count);
        if (fgets(input_command, sizeof(input_command), stdin) == NULL) 
        {
            perror("fgets");
            return 1;
        }
        input_command[strcspn(input_command, "\n")] = 0;  
        int input_length = strlen(input_command);
        if (strstr(input_command, "log") == NULL) 
        {
            add_command_to_log(&log,input_command);
        }
        save_log(&log, "log1.txt", home);
        for (int i = 0; i < strlen(input_command);) 
        {
            if (input_command[i] == '&' || input_command[i] == ';') 
            {
                if (input_command[i] == '&') 
                {
                    strncpy(background_command, input_command, i);
                    background_command[i] = '\0';
                    trim_command(background_command);
                    if (strlen(background_command) > 0) 
                    {
                        execute_background_command(background_command);
                    }
                    memmove(input_command, input_command + i + 1, strlen(input_command) - i);
                } 
                else 
                {
                    strncpy(foreground_command, input_command, i);
                    foreground_command[i] = '\0';
                    trim_command(foreground_command);
                    if (strlen(foreground_command) > 0) 
                    {
                        execute_foreground_command(foreground_command, &log, home, prev_direc, fg_commands, &fg_count);
                    }
                    memmove(input_command, input_command + i + 1, strlen(input_command) - i);
                }
                input_length = strlen(input_command);
                i = 0;
            } 
            else 
            {
                i++;
            }
        }
        trim_command(input_command);
        if (strlen(input_command) > 0) 
        {
            execute_foreground_command(input_command, &log, home, prev_direc, fg_commands, &fg_count);
        }
    }
    return 0;
}
