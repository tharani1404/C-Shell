#include"headers.h"
void prompt_user(char* home, FG* fg_commands, int fg_count) 
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) 
    {
        perror("getpwuid");
        return;
    }
    char* username = pw->pw_name;
    char systemname[4096];
    if (gethostname(systemname, sizeof(systemname)) != 0) 
    {
        perror("gethostname");
        return;
    }
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
    {
        perror("getcwd");
        return;
    }
    if (strncmp(cwd, home, strlen(home)) == 0) 
    {
        printf("\033[32m<%s@%s\033[0m:\033[34m~%s\033[0m", 
               username, systemname, cwd + strlen(home));
    } 
    else 
    {
        printf("\033[32m<%s@%s\033[0m:\033[34m%s\033[0m", username, systemname, cwd);
    }

    if (fg_count > 0) 
    {
        printf(" [");
        for (int i = 0; i < fg_count; ++i) 
        {
            printf("%s (%dsec)", fg_commands[i].command, fg_commands[i].time);
            if (i < fg_count - 1) 
            {
                printf(", ");
            }
        }
        printf("]");
    }
    if(fg_count>1)
    {
        int t=0;
        for(int i=0;i<fg_count;i++)
        {
            t=t+fg_commands[i].time ;
        }
        printf(" (%ds)",t);
    }

    printf("> ");
}

void change_directory(char *path, char *home, char *prev_direc) 
{
    char new_dir[4096];
    char current_dir[4096];
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    char *username = pw->pw_name;

    path=strtok(path,"/");
    
    while(path!=NULL)
    {
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) 
        {
            perror("getcwd");
            return;
        }
        if(strcmp(path,"~")==0)
        {
            strcpy(new_dir,home);
        }
        else if (strcmp(path,"-")==0)
        {
            if (strlen(prev_direc) == 0) 
            {
                printf("\033[31mERROR:\033[0m No previous directory.\n");
                return;
            }
            strcpy(new_dir, prev_direc);
        }
        else if(strcmp(path,"..")==0)
        {
            if (getcwd(new_dir, sizeof(new_dir)) == NULL) 
            {
                perror("getcwd");
                return;
            }
            char *last_slash = strrchr(new_dir, '/');
            if (last_slash != NULL) 
            {
                *last_slash = '\0';
            }
        }
        else if(strcmp(path,".")==0)
        {
            if(getcwd(new_dir,sizeof(new_dir))==NULL)
            {
                perror("getcwd");
                return;
            }
        }
        else if(path[0]=='/')
        {
            if (getcwd(new_dir, sizeof(new_dir)) == NULL) 
            {
                perror("getcwd");
                return;
            }
            strcat(new_dir, path);
        }
        else
        {
            char temp[4000];
            if (getcwd(temp, sizeof(temp)) == NULL) 
            {
                perror("getcwd");
                return;
            } 
            snprintf(new_dir,sizeof(new_dir),"%s/%s",temp,path);
        }
        if (chdir(new_dir) != 0) 
        {
            printf("\033[31mERROR:\033[0m %s No such file or directory\n",new_dir);
            return;
        }
        else
        {
        strcpy(prev_direc, current_dir);
        }
        path=strtok(NULL,"/");
    }
    printf("%s\n", new_dir); 
}