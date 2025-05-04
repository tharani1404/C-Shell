#include "headers.h"

void set_flags(char* flags, char* home, int* l, int* a, char* n_dir, char* prev_dir, int p,int k) 
{
    char current_dir[PATH_MAX];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) 
    {
        perror("getcwd");
        return;
    }

    if (flags[0] == '-') 
    {
        int n = strlen(flags);
        for (int i = 1; i < n; i++) 
        {
            if (flags[i] == 'l') *l = 1;
            else if (flags[i] == 'a') *a = 1;
            else 
            {
                printf("\033[31mERROR:\033[0m Invalid flag: %c\n", flags[i]);
                return;
            }
        }

        if (p == 1) 
        {
            if (strlen(prev_dir) == 0) 
            {
                printf("\033[31mERROR:\033[0m No previous directory.\n");
                return;
            }
            strcpy(n_dir, prev_dir);
        }
    } 
    else 
    {
        if (flags[0] != '/') 
        {
            strcpy(n_dir, current_dir);
            strcat(n_dir, "/");
        } 

        char* token = strtok(flags, "/");
        while (token != NULL) 
        {
            if (strcmp(token, "~") == 0) 
            {
                strcpy(n_dir, home);
            }
            else if (strcmp(token, "-") == 0) 
            {
                if (strlen(prev_dir) == 0) 
                {
                    printf("\033[31mERROR:\033[0m No previous directory.\n");
                    return;
                }
                strcpy(n_dir, prev_dir);
            }
            else if (strcmp(token, "..") == 0) 
            {
                strcpy(n_dir,current_dir);
            char *last_slash = strrchr(n_dir, '/');
            if (last_slash != NULL) 
            {
                *last_slash = '\0';
            }
            }
            else if (strcmp(token, ".") == 0)
            {
                strcpy(n_dir,current_dir);
            }
            else
            {
                char temp[4096];
                strcpy(temp,current_dir);
                snprintf(n_dir,k,"%s/%s",temp,token);
            }
            strcpy(current_dir,n_dir);
            token = strtok(NULL, "/");
        }
    }
}





void file_info(struct stat* file_stat, char* filename) {
    if (file_stat == NULL || filename == NULL) 
    {
        printf("\033[31mERROR:\033[0m Invalid file information.\n");
        return;
    }

    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
    printf(" %ld", (long)file_stat->st_nlink);

    struct passwd* pwd = getpwuid(file_stat->st_uid);
    struct group* grp = getgrgid(file_stat->st_gid);
    if (pwd == NULL || grp == NULL) 
    {
        printf("\033[31mERROR:\033[0m Unable to retrieve user or group information.\n");
        return;
    }
    printf(" %s %s", pwd->pw_name, grp->gr_name);
    printf(" %ld", (long)file_stat->st_size);
    char timebuf[80];
    struct tm* timeinfo = localtime(&file_stat->st_mtime);
    if (timeinfo == NULL) {
        printf("\033[31mERROR:\033[0m Failed to retrieve local time.\n");
        return;
    }
    if (strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo) == 0) {
        printf("\033[31mERROR:\033[0m Failed to format time.\n");
        return;
    }
    if (S_ISDIR(file_stat->st_mode)) {
        printf(" %s \033[34m%s\033[0m\n", timebuf, filename);  
    } else if (S_ISREG(file_stat->st_mode) && (file_stat->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        printf(" %s \033[32m%s\033[0m\n", timebuf, filename);  
    } else {
        printf(" %s %s\n", timebuf, filename);  
    }
}

void reveal_files(char* n_dir, int l, int a) 
{
    struct stat file_stat;
    if (stat(n_dir, &file_stat) == -1) 
    {
        printf("\033[31mERROR:\033[0m Unable to access '%s': %s\n", n_dir, strerror(errno));
        return;
    }
    if (S_ISREG(file_stat.st_mode)) 
    {
        char new[4096];
        if (getcwd(new, sizeof(new)) == NULL) 
        {
            perror("getcwd");
            return;
        }
        char* filename = n_dir + strlen(new) + 1;
        if (l == 1) 
        {
            file_info(&file_stat, filename);
        } 
        else 
        {
            printf("%s\n", filename);
        }
        return;
    }

    struct dirent** namelist;
    int n = scandir(n_dir, &namelist, NULL, alphasort);
    if (n < 0) 
    {
        printf("\033[31mERROR:\033[0m Unable to read directory '%s': %s\n", n_dir, strerror(errno));
        return;
    }
    printf("Total number of files or directories: %d\n", n);
    for (int i = 0; i < n; i++) 
    {
        if (a == 0 && namelist[i]->d_name[0] == '.') 
        {
            free(namelist[i]);
            continue;
        }
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", n_dir, namelist[i]->d_name);
        if (stat(path, &file_stat) == -1) 
        {
            printf("\033[31mERROR:\033[0m Unable to access '%s': %s\n", path, strerror(errno));
            free(namelist[i]);
            continue;
        }
        
        if (l != 1) 
        {
            if (S_ISDIR(file_stat.st_mode)) 
            {
                printf("\033[34m%s\033[0m\n", namelist[i]->d_name);  
            } 
            else if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) 
            {
                printf("\033[32m%s\033[0m\n", namelist[i]->d_name);  
            } 
            else 
            {
                printf("%s\n", namelist[i]->d_name);  
            }
        }

        if (l == 1) 
        {
            file_info(&file_stat, namelist[i]->d_name);
        }
        free(namelist[i]);
    }
    free(namelist);
}

