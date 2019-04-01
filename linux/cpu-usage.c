// gcc cpu-usage.c -o cpu-usage

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

struct cpu_usage {
    uint64_t out[10];
    uint64_t old_idle;
    uint64_t old_sum;
};

struct stat_parser {
    char buf[256];
    int status;
    int i;
    int total;
    int pos;
    int cpu;
};

enum {
    p_read,
    p_parser
};

int get_next_cpustat(uint64_t out[10], struct stat_parser *p, int fd){
    ssize_t n;
    char c;

    memset(out, 0x0, sizeof(uint64_t)*10);
    p->pos = 0;

    while(p->pos != 10){
        if(p->status == p_read){
            n = read(fd, p->buf, sizeof(p->buf));
            if(n <= 0){
                goto end;
            }

            p->buf[n] = 0x0;

            p->status = p_parser;
            p->total = n;
            p->i = 0;
        }

        while(p->i < p->total){
            c = p->buf[p->i++];

            // check /^cpu/
            if(p->cpu < 3){
                if(c != "cpu"[p->cpu++]){
                    goto end;
                }
            }

            // skip cpuN
            else if(p->cpu == 3){
                if(c == ' ')
                    p->cpu++;
            }

            // skip spaces
            else if(p->cpu == 4){
                if(c != ' '){
                    p->cpu++;
                    p->i--;
                }
            }

            else {
                if(c >= '0' && c <= '9'){
                    out[p->pos] *= 10;
                    out[p->pos] += c-'0';
                } else {
                    if(++p->pos == 10){
                        p->cpu = 0;
                        //p->status = p_read;
                        goto end;
                    }
                }
            }
        }

        p->status = p_read;
    }

    end:
    return (p->pos == 10);
}

float get_usage_percent(struct cpu_usage *stat){
    uint64_t idle, sum;
    int i;

    idle = stat->out[3];
    sum = 0;

    for(i=0; i<10; i++){
        sum += stat->out[i];
    }

    float usage = (1.0-(idle-stat->old_idle)*1.0/(sum-stat->old_sum))*100;

    stat->old_sum = sum;
    stat->old_idle = idle;

    return usage;
}

int proc_stat_loop(int fd){
    // you may need to reopen the file in old kernel
    return (lseek(fd, 0, SEEK_SET) != -1);
}

int main(void){
    struct cpu_usage *usage;
    struct stat_parser p;
    int fd, i, n;

    n = get_nprocs();

    usage = calloc(n+1, sizeof(struct cpu_usage));
    if(usage == NULL){
        perror("calloc()");
        return 1;
    }

    fd = open("/proc/stat", O_RDONLY);
    if(fd == -1){
        perror("open()");
        return 1;
    }

    do {
        p.status = p_read;
        p.cpu = 0;

        get_next_cpustat(usage[0].out, &p, fd);
        printf("total   %10.2f\n", get_usage_percent(usage));

        for(i=0; i<n && get_next_cpustat(usage[i+1].out, &p, fd); i++){
            printf("cpu%-4d %10.2f\n", i, get_usage_percent(usage+i+1));
        }

        sleep(2);

        // move cursor n lines up
        printf("\e[%dA", i+1);

    } while(proc_stat_loop(fd));

    return 0;
}
