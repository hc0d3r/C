// gcc whois_br.c -o whois_br

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define WHOIS "registro.br"

int whois(char *hostname){
    struct sockaddr_in addr;
    struct hostent *host;
    int fd, nbytes, ret = 1;
    char buf[1024];


    size_t len = strlen(hostname);
    hostname[len++] = '\n';

    fd = socket(AF_INET,SOCK_STREAM,0);

    if(fd == -1){
        perror("failed to create socket");
        goto end;
    }

    host = gethostbyname(WHOIS);

    addr.sin_family = host->h_addrtype;
    addr.sin_port   = htons(43);
    addr.sin_addr   = *((struct in_addr *)host->h_addr);

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
        perror("connection failed");
        goto end;
    }

    if(send(fd, hostname, len, 0) == -1){
        perror("failed to send hostname");
        goto end;
    }

    while((nbytes = recv(fd, buf, sizeof(buf), 0)) > 0){
        write(STDOUT_FILENO, buf, nbytes);
    }

    ret = 0;

    end:
    return ret;
}

int main(int argc, char **argv){

    if(argc != 2){
        puts("usage: whois_br [domain.br]");
        return 0;
    }

    return whois(argv[1]);
}
