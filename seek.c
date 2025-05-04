#include"headers.h"
void set_flags_for_seek(char*command,int*d,int*f,int*e,char*name,char*dir,char*home,char*prev_dir,int p,int k,int *w)
{
    char current_dir[PATH_MAX];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) 
    {
        perror("getcwd");
        return;
    }

    if(command[0]=='-')
    {
        int n=strlen(command);
        for(int i=0;i<n;i++)
        {
            if (command[i] == 'd') *d = 1;
            if (command[i] == 'f') *f = 1;
            if (command[i] == 'e') *e = 1;
        }
        if (p == 1) 
        {
            if (strlen(prev_dir) == 0) 
            {
                fprintf(stderr, "No previous directory.\n");
                
                return;
            }
            strcpy(dir, prev_dir);
        }
    }
    else
    {
        if(*w==0)
        {
            strcpy(name,command);
            (*w)++;
        }
        else
        {
        if (command[0] != '/') {
            strcpy(dir, current_dir);
            strcat(dir, "/");
        } 

        char* token = strtok(command, "/");
        while (token != NULL) 
        {
            if (strcmp(token, "~") == 0) 
            {
                strcpy(dir, home);
            }
            else if (strcmp(token, "-") == 0) 
            {
                if (strlen(prev_dir) == 0) 
                {
                    printf("\033[31mERROR:\033[0m No previous directory.\n");
                    return;
                }
                strcpy(dir, prev_dir);
            }
            else if (strcmp(token, "..") == 0) 
            {
                strcpy(dir,current_dir);
            char *last_slash = strrchr(dir, '/');
            if (last_slash != NULL) 
            {
                *last_slash = '\0';
            }
            }
            else if (strcmp(token, ".") == 0)
            {
                strcpy(dir,current_dir);
            }
            else
            {
                char temp[4096];
                strcpy(temp,current_dir);
            snprintf(dir,k,"%s/%s",temp,token);
            }
            strcpy(current_dir,dir);
            token = strtok(NULL, "/");
        }
        }
    }
    
}

void seek_file(int d, int f, int e, char* name, char* dir, char* initial_dir,int*file_count,int*dir_count,char*ret_direc) 
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) 
    {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dp)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
        {
            continue;
        }

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        if (stat(path, &statbuf) == -1) 
        {
            perror("stat");
            continue;
        }

        if (strncmp(entry->d_name, name, strlen(name)) == 0) 
        {
            const char *relative_path = path + strlen(initial_dir);
            char display_path[4096];
            snprintf(display_path, sizeof(display_path), ".%s", relative_path);

            if (S_ISDIR(statbuf.st_mode) && !f) 
            {
                if (d || !f) 
                {
                    *dir_count=*dir_count+1;
                    strcpy(ret_direc,display_path);
                    printf("\033[34m%s\033[0m\n", display_path);  
                }
            } 
            else if (S_ISREG(statbuf.st_mode) && !d) 
            {
                *file_count=*file_count+1;
                strcpy(ret_direc,display_path);
                printf("\033[32m%s\033[0m\n", display_path);  
            }
        }
        if (S_ISDIR(statbuf.st_mode)) 
        {
            seek_file(d, f, e, name, path, initial_dir,file_count,dir_count,ret_direc);
        }
    }
    
    closedir(dp);
}
void handle_e_flag(int e,int file_count,int dir_count,char*dir,char*ret_direc,int k)
{
    if(e==1)
    {
        char new_direc[4096];
        snprintf(new_direc, sizeof(new_direc), "%s%s", dir, ret_direc + 1);
        if(file_count+dir_count==1)
        {
            if(dir_count==1)
            {
                if (chdir(new_direc) != 0) 
                {
                    perror("chdir");
                    return;
                }
            }
            else if(file_count==1)
            {
                char path[4096];
                snprintf(path, sizeof(path), "%s/%s", dir, ret_direc+2);
                FILE*fp=fopen(path,"r");
                if(fp==NULL)
                {
                    perror("fopen");
                    return;
                }
                char buffer[1024];
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) 
                {
                    fwrite(buffer, 1, bytesRead, stdout);
                }
                printf("\n");
                if (ferror(fp)) 
                {
                    perror("fread");
                }
                fclose(fp);
            }
        }
    }
}


