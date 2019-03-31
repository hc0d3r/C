#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void check_host(const char *host){
    static const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_CANONNAME,
        .ai_protocol = 0
    };

    struct addrinfo *addrlist = NULL;
    char ip[INET6_ADDRSTRLEN];
    void *aux;

    if(getaddrinfo(host, NULL, &hints, &addrlist)){
        printf(" '--- unknow host\n");
        goto end;
    }

    for(; addrlist; addrlist = addrlist->ai_next){

        if(addrlist->ai_family == AF_INET){
            aux = &((struct sockaddr_in *)addrlist->ai_addr)->sin_addr;
        } else {
            aux = &((struct sockaddr_in6 *)addrlist->ai_addr)->sin6_addr;
        }

        inet_ntop(addrlist->ai_family, aux, ip, INET6_ADDRSTRLEN);

        printf(" '--- %s\n", ip);
    }

    end:
    freeaddrinfo(addrlist);
}

int main(int argc, char *argv[]){
    size_t hlen, slen, len, size;
    ssize_t n;

    char *ptr, *subdomain, *hostname;
    FILE *fh;

    if(argc != 3){
        printf("usage: subdomain [hostname] [wordlist]\n");
        return 0;
    }

    if((fh = fopen(argv[2], "r")) == NULL){
        perror("fopen()");
        return 1;
    }

    hostname = argv[1];

    hlen = strlen(hostname);
    slen = size = 0;
    subdomain = ptr = NULL;

    while((n = getline(&ptr, &size, fh)) > 0){
        if(n == 1 && ptr[0] == '\n'){
            continue;
        }

        if(ptr[n-1] == '\n'){
            ptr[n--] = 0x0;
        }

        len = n+hlen;

        if(ptr[n] != '.' && hostname[0] != '.'){
            len++;
        }

        if(slen < len){
            subdomain = realloc(subdomain, len+1);
            slen = len;
        }

        memcpy(subdomain, ptr, n);
        if(ptr[n] != '.' && hostname[0] != '.'){
            subdomain[n++] = '.';
        }

        memcpy(subdomain+n, hostname, hlen);
        subdomain[len] = 0x0;

        printf("[+] %s\n", subdomain);
        check_host(subdomain);
        printf("\n\n");

    }

    fclose(fh);
    free(subdomain);
    free(ptr);

    return 0;
}
