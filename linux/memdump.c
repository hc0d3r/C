#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


#define check_overflow (INT_MAX/10)
#define xfree(x) safefree((void **)&(x))

#define copy_str(dest, source, start, end, i, j) \
    for(j=0, i=start; i<end; i++,j++) \
        dest[j] = source[i]; \
    dest[j] = 0;

typedef struct maps_addrs {
    off_t start;
    off_t end;
    char *pathname;
    struct maps_addrs *next;
} maps_addrs_t;

typedef struct x {
    int first_end;
    int second_end;
    int pathname_ini;
    int pathname_end;
} elements_position_t;

int parse_line(const char *buf, elements_position_t *elements);

void safefree(void **pp){
    if(pp != NULL){
        free(*pp);
        *pp = NULL;
    }
}

char hexchar(char x){
    if(x >= '0' && x <= '9')
        return x-'0';

    else if(x >= 'a' && x <='f')
        return x-'a'+10;

    else if(x >= 'A' && x <='F')
        return x-'A'+10;

    else
        exit(1);
}

void hexstring2off_t(off_t *out, const char *hstr, int start){
    int i, j = 0;

    for(i=0; i<=start; i++, j++){
        (*out) = (*out << 4) | hexchar(hstr[i]);
    }

}

int str2pid_t(pid_t *output, const char *strnum, char **error){
    int i;
    char aux;
    *output = 0;

    for(i=0; strnum[i]; i++){
        aux = strnum[i] - '0';

        if( aux < 0 || aux > 9 ){
            if(error){
                *error = "Invalid number";
            }
            return 0;
        }

        if( *output > check_overflow ){
            if(error){
                *error = "Integer overflow";
            }
            return 0;
        } else {
            *output *= 10;
        }

        if( *output > (INT_MAX-aux) ){
            if(error){
                *error = "Integer overflow";
            }
        } else {
            *output += aux;
        }
    }

    return 1;

}

void free_addr_list(maps_addrs_t *addr){
    maps_addrs_t *aux;

    while(addr != NULL){
        aux = addr->next;
        xfree(addr->pathname);
        xfree(addr);
        addr = aux;
    }

}

void get_addr_list(maps_addrs_t **addrs, FILE *fh){
    char line_buf[1024];
    int i = 0, j;

    maps_addrs_t *novo, **aux;

    aux = addrs;


    elements_position_t elements;

    while( fgets(line_buf, sizeof(line_buf), fh) ){
        if( parse_line(line_buf, &elements) ){
            novo = calloc(1,sizeof(maps_addrs_t));

            if(elements.pathname_ini && elements.pathname_end){
                novo->pathname = malloc(1+(elements.pathname_end-elements.pathname_ini));
                copy_str(novo->pathname, line_buf, elements.pathname_ini, elements.pathname_end, i, j);
            }

            hexstring2off_t(&(novo->start), line_buf, elements.first_end-1);
            hexstring2off_t(&(novo->end), line_buf+elements.first_end+1, (elements.second_end - elements.first_end)-2);

            *aux = novo;
            aux = &(novo->next);

        }
    }

}

int parse_line(const char *buf, elements_position_t *elements){

    int i, space_cout = 0;

    memset(elements, '\0', sizeof(elements_position_t));

    for(i=0; buf[i]; i++){
        if(buf[i] == '-'){
            elements->first_end = i;
            i++;
            break;
        }
    }

    for(; buf[i]; i++){
        if(buf[i] == ' '){
            elements->second_end = i;
            i++;
            break;
        }
    }


    for(; buf[i]; i++){
        if(buf[i] == ' '){
            space_cout++;
        }

        else {
            if( space_cout > 5 ){
                elements->pathname_ini = i;
                i++;
                break;
            }
        }
    }

    for(; buf[i]; i++){
        if(buf[i] == '\n'){
            elements->pathname_end = i;
        }
    }

    return (elements->first_end && elements->second_end);

}


int main(int argc, char **argv){
    char *strerror, filepath[30], *buf;
    maps_addrs_t *addrs = NULL, *aux;
    pid_t pid;
    FILE *fh;
    int fd, self = 0;
    size_t size;
    ssize_t len;

    if(argc != 3){
        fprintf(stderr, "memdump: <pid> <pathname>\n");
        return 0;
    }

    if( !strcmp(argv[1], "self") ){
        self = 1;
    } else {
        if(! str2pid_t(&pid, argv[1], &strerror) ){
            fprintf(stderr, "error -> %s\n", strerror );
            return 1;
        }
    }

    if(self){
        strcpy(filepath, "/proc/self/maps");
    } else {
        snprintf(filepath, sizeof(filepath), "/proc/%d/maps", pid);
    }

    if( (fh = fopen(filepath, "r")) == NULL ){
        perror("Failed to open file");
        return 1;
    }

    if(self){
        strcpy(filepath, "/proc/self/mem");
    } else {
        snprintf(filepath, sizeof(filepath), "/proc/%d/mem", pid);
    }

    get_addr_list(&addrs, fh);
    fclose(fh);

    for(aux=addrs; aux!=NULL; aux=aux->next){
        if(!aux->pathname) continue;

        if(!strcmp(argv[2], aux->pathname)){

            if(!self){
                ptrace(PTRACE_ATTACH, pid, NULL, NULL);
                waitpid(pid, NULL, 0);
            }

            fd = open(filepath, O_RDONLY);

            size = (size_t)(aux->end - aux->start);
            if( (buf = malloc(size)) == NULL){
                perror("malloc()");
                exit(1);
            }

            lseek(fd, aux->start, SEEK_SET);
            len = read(fd, buf, size);

            if(!self) ptrace(PTRACE_DETACH, pid, NULL, NULL);

            write(1, buf, len);
            close(fd);
            free(buf);
            break;
        }
    }

    free_addr_list(addrs);

    return 0;
}


