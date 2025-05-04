#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include<limits.h>
#include<ctype.h>
#include <math.h>
#define MAX_C 15
#define MAX_CL 4096
struct Command{
    char commands[MAX_C][MAX_CL];
    int start;
    int count;
};
typedef struct Command commandlog;
struct ProcessList{
    int pid;
    char name[4096];
    //int count;
};
struct fg{
    char*command;
    int time;
};
typedef struct fg FG;
typedef struct ProcessList processlist;
extern processlist *global_process_list;
extern int *global_process_count;
void prompt_user(char*home,FG*fg_commands,int fg_count);
void change_directory(char*path,char*home,char*prev_direc);
void set_flags(char*flags,char*home,int*l,int*a,char*new_dir,char*prev_dir,int p,int k);
void reveal_files(char*new_dir,int l,int a);
void file_info(struct stat*file_stat,char*filename);
void add_command_to_log(commandlog*log,char*command);
void print_log(commandlog*log);
void log_purge(commandlog*log);
void load_log(commandlog*log,char*filename);
void save_log(commandlog*log,char*filename,char*home);
void remove_log_commands(char *input_command);
void trim_command(char*command);
void execute_background_command(char*background_command);
void execute_foreground_command(char*foreground_command,commandlog*log,char*home,char*prev_dir,FG*fg_commands,int*fg_count);
void add_process_list(int pid,char*background_command);
void handle_sigchld(int sig);
void execute_command(commandlog*log,int n,char*home,char*prev_direc,FG*fg_commands,int*fg_count);
void clear_fg_commands(FG* fg_commands, int* fg_count);
void proclore(int pid,char*home);
void set_flags_for_seek(char*command,int *d,int *f,int*e,char*name,char*dir,char*home,char*prev_dir,int p,int k,int *w);
void seek_file(int d,int f,int e,char*name,char*dir,char*initial_dir,int*file_count,int*dir_count,char*ret_direc);
void handle_e_flag(int e,int flag_count,int dir_count,char*dir,char*ret_direc,int k);
#endif