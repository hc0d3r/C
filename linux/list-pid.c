#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>

int print_cmdline(pid_t pid){
    char buf[1024];

    ssize_t n, i;
    int fd, ret = 0;

    sprintf(buf, "/proc/%d/cmdline", pid);
    if((fd = open(buf, O_RDONLY)) == -1)
        goto end;

    while((n = read(fd, buf, sizeof(buf))) > 0){
        for(i=0; i<n; i++){
            char c = buf[i];
            putchar(c ? c : ' ');
        }
        ret += n;
    }

    close(fd);

    end:
    return ret;
}

void print_status(pid_t pid){
    char buf[1024];

    ssize_t n, i;
    int fd;

    sprintf(buf, "/proc/%d/status", pid);
    if((fd = open(buf, O_RDONLY)) == -1)
        return;

    lseek(fd, 6, SEEK_SET);

    putchar('[');

    while((n = read(fd, buf, sizeof(buf))) > 0){
        for(i=0; i<n; i++){
            if(buf[i] == '\n')
                goto end;

            putchar(buf[i]);
        }
    }

    end:
    putchar(']');

    close(fd);

}

pid_t *getpids(void){
    pid_t *ret = NULL;
    size_t len = 0;

    struct dirent *dirp;
    DIR *dir;

    if((dir = opendir("/proc")) == NULL)
        goto end;

    while((dirp = readdir(dir))){
        if(dirp->d_type != DT_DIR ||
          (dirp->d_name[0] < '0' || dirp->d_name[0] > '9'))
            continue;

        ret = realloc(ret, (len+1)*sizeof(pid_t));
        if(ret == NULL)
            break;

        ret[len++] = atoi(dirp->d_name);
    }

    closedir(dir);

    end:
    return ret;
}

int main(void){
    struct passwd *pw;
    struct stat st;

    char filename[32];
    pid_t *pids, pid;

    if((pids = getpids()) == NULL){
        perror("getpids()");
        return 1;
    }

    printf("USER\tPID\tCOMMAND\n");
    while((pid = *pids++)){
        sprintf(filename, "/proc/%d", pid);

        if(stat(filename, &st) == -1){
            continue;
        }

        if((pw = getpwuid(st.st_uid)) == NULL){
            continue;
        }

        printf("%s\t", pw->pw_name);
        printf("%d\t", pid);

        if(!print_cmdline(pid)){
            print_status(pid);
        }

        putchar('\n');
    }

    return 0;
}
