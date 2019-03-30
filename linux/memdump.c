#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define warn(x...) fprintf(stderr, x);

struct maps {
    off_t start;
    off_t end;
    char perms[5];
    struct maps *next;
};

void get_maps_list(struct maps **out, FILE *fh){
    char *line = NULL, *end, *perms;
    size_t size = 0;

    while(getline(&line, &size, fh) > 0){
        if((*out = malloc(sizeof(struct maps))) == NULL){
            perror("malloc()");
            exit(1);
        }

        (*out)->start = strtol(line, &end, 16);
        (*out)->end = strtol(end+1, &perms, 16);
        memcpy((*out)->perms, perms+1, 4);
        (*out)->perms[4] = 0x0;

        (*out)->next = NULL;
        out = &((*out)->next);
    }

    free(line);
}

void free_maps_list(struct maps *list){
    struct maps *aux;

    while(list){
        aux = list->next;
        free(list);
        list = aux;
    }
}

void check_ptrace_scope(void){
    FILE *fh;
    char c;

    if((fh = fopen("/proc/sys/kernel/yama/ptrace_scope", "r")) == NULL){
        warn("can't open /proc/sys/kernel/yama/ptrace_scope\n");
        goto end;
    }

    if((c = fgetc(fh)) == EOF){
        warn("can't read /proc/sys/kernel/yama/ptrace_scope\n");
        goto end;
    }

    if(c != '0'){
        warn("ptrace_scope = %c, the program probably will not work properly\n", c);
    }


    end:
    fclose(fh);
}

int main(int argc, char **argv){
    struct maps *maps = NULL, *aux;
    char *ptr, filename[36], *outname, *dump;
    FILE *fh;

    int fd, memfd = -1;
    pid_t pid;
    off_t len;

    if(argc != 3){
        printf("memdump [pid] [output-folder]\n");
        return 0;
    }

    pid = strtol(argv[1], NULL, 0);

    if(pid){
        check_ptrace_scope();
        sprintf(filename, "/proc/%d/maps", pid);

        ptr = filename;
    } else {
        ptr = "/proc/self/maps";
    }

    if((fh = fopen(ptr, "r")) == NULL){
        perror("fopen()");
        return 1;
    }

    get_maps_list(&maps, fh);
    fclose(fh);

    if(maps == NULL){
        warn("no maps found in /proc/[pid]/maps\n");
        return 1;
    }

    if(pid){
        if(ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1){
            perror("ptrace()");
            goto end;
        }

        if(waitpid(pid, NULL, 0) == -1){
            perror("waitpid()");
            goto end;
        }

        sprintf(filename, "/proc/%d/mem", pid);
    } else {
        ptr = "/proc/self/mem";
    }

    memfd = open(ptr, O_RDONLY);
    if(memfd == -1){
        perror("open()");
        goto end;
    }

    outname = malloc(strlen(argv[2])+64);
    if(outname == NULL){
        perror("malloc()");
        goto end;
    }

    ptr = outname;

    for(aux = maps; aux != NULL; aux = aux->next){
        sprintf(outname, "%s/%lx-%lx-%s.dump", argv[2], aux->start,
            aux->end, aux->perms);

        len = aux->end-aux->start;

        if((dump = malloc(len)) == NULL){
            perror("malloc()");
            goto end;
        }

        printf("dumping %lx-%lx ...\n", aux->start, aux->end);

        if(pread(memfd, dump, len, aux->start) == -1){
            free(dump);
            continue;
        }

        if((fd = open(ptr, O_RDWR|O_CREAT|O_TRUNC, 0644)) == -1){
            warn("failed to create/overwrite %s\n", outname);
            continue;
        }
        write(fd, dump, len);
        close(fd);

        free(dump);
    }

    free(outname);

    end:

    if(memfd != -1)
        close(memfd);

    if(pid && memfd != -1)
        ptrace(PTRACE_DETACH, pid, NULL, NULL);

    free_maps_list(maps);
    return 0;
}
