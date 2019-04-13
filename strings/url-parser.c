// gcc url-parser.c -o url-parser

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_URL_T (url_t){ NULL, NULL, 0, NULL };

typedef struct url {
    char *scheme;
    char *domain;
    uint16_t port;
    char *path;
} url_t;


int url_parser(url_t *url, const char *str){
    int scheme = 0,
        hstart = 0,
        hend = 0,
        port = 0,
        path = 0;

    int i, ret = 1, len;
    char *aux, c;
    long int tmp;

    if((aux = strstr(str, "://"))){
        scheme = aux-str;
        hstart = scheme+3;
    }

    for(i=hstart; (c = str[i]); i++){
        if(c == '/'){
            hend = i;
            path = i;
            break;
        }

        else if(c == ':'){
            hend = i;
            port = i+1;
            break;
        }
    }

    if(!hend){
        hend = i;
    }

    if(hstart == hend){
        goto end;
    }

    if(port){
        tmp = strtol(str+port, &aux, 10);
        if(tmp <= 0 || tmp > 0xffff)
            goto end;

        url->port = (uint16_t)tmp;

        if(*aux == '/'){
            path = aux-str;
        }

        else if(*aux != 0x0){
            goto end;
        }
    }

    if(scheme){
        len = scheme;
        url->scheme = malloc(len+1);
        memcpy(url->scheme, str, len);
        url->scheme[len] = 0x0;
    }

    len = hend-hstart;
    url->domain = malloc(len+1);
    memcpy(url->domain, str+hstart, len);
    url->domain[len] = 0x0;

    if(path){
        len = strlen(str)-path;
        url->path = malloc(len+1);
        memcpy(url->path, str+path, len);
        url->path[len] = 0x0;
    }

    ret = 0;

    end:
    return ret;
}

void url_free(url_t *url){
    free(url->scheme);
    free(url->domain);
    free(url->path);
}

int main(int argc, char *argv[]){
    url_t myurl = DEFAULT_URL_T;

    if(argc != 2){
        printf("url-parser [url]\n");
        return 0;
    }

    if(url_parser(&myurl, argv[1])){
        printf("invalid url\n");
        return 1;
    }

    printf("scheme: %s\n", myurl.scheme);
    printf("domain: %s\n", myurl.domain);
    printf("port: %d\n", myurl.port);
    printf("path: %s\n", myurl.path);

    url_free(&myurl);

    return 0;
}
