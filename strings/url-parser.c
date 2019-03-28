/*
function to parser url (~:
Coder: mmxm (@hc0d3r)
*/

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_STR_T (str_t){ NULL, 0 }
#define DEFAULT_URL_T (url_t){ DEFAULT_STR_T, DEFAULT_STR_T, DEFAULT_STR_T, 0 };

#define MAX_SHORT 0xffff

#define xfree(x) _xfree((void **)&x)

#define free_str_t(x) xfree(x.ptr); \
        x.len = 0;

typedef enum {
    NO_ERROR,
    INVALID_HOSTNAME,
    INVALID_PORT_NUMBER,
    INTEGER_OVERFLOW,
    SIZE_PARSER_ERRS
} url_parser_err;


typedef struct {
    char *ptr;
    size_t len;
} str_t;


typedef struct {
    str_t protocol;
    str_t hostname;
    str_t path;
    unsigned short port;
} url_t;


void die(const char *err){
    perror(err);
    abort();
}

void *xmalloc(size_t len){
    void *ptr = malloc(len);

    if(ptr == NULL)
        die("malloc()");

    return ptr;
}

void _xfree(void **ptr){
    if(ptr != NULL){
        free(*ptr);
        *ptr = NULL;
    }
}

const char *url_parser_strerror(url_parser_err errcode){
    const char *errors[SIZE_PARSER_ERRS] = {
        "Success", "Invalid Hostname", "Invalid Port Number", "Integer overflow"
    };

    return errors[errcode];
}

int url_parser(url_t *url, const char *str, url_parser_err *errcode){
    static const char *scheme = "://";

    int scheme_end = 0, i, j, k,
    host_start = 0, port_start = 0, host_end = 0, path_start = 0, path_end = 0;

    unsigned short tmp = 0;

    *errcode = NO_ERROR;

    for(i=0; str[i]; i++){
        for(j=i,k=0; scheme[k] == str[j]; j++, k++){
            if(scheme[k+1] == 0){
                scheme_end = i;
                host_start = i+3;
            }
        }
    }

    for(i=host_start; str[i]; i++){
        if(str[i] == '/'){
            path_start = i;
            break;
        } else if(str[i] == ':'){
            port_start = i+1;
            break;
        } else {
            host_end = i+1;
        }
    }


    if(!host_end){
        *errcode = INVALID_HOSTNAME;
        return 0;
    }

    if(port_start){
        for(i=port_start; str[i]; i++){

            if(str[i] == '/'){
                path_start = i;
                break;
            }

            else if(str[i] < '0' || str[i] > '9'){
                *errcode = INVALID_PORT_NUMBER;
                return 0;
            }

            if( tmp  > (MAX_SHORT/10) ){
                *errcode = INTEGER_OVERFLOW;
                return 0;
            } else {
                tmp *= 10;
            }

            if( tmp > ( MAX_SHORT - (str[i] - '0') ) ){
                *errcode = INTEGER_OVERFLOW;
                return 0;
            } else {
                tmp += (str[i] - '0');
            }

        }
    }

    url->port = tmp;

    if(path_start){

        for(i=path_start; str[i]; i++);
        path_end = i;

        url->path.len = (size_t) (path_end-path_start);
        url->path.ptr = xmalloc( url->path.len + 1);

        for(i=path_start, j = 0; str[i]; i++, j++){
            url->path.ptr[j] = str[i];
        }

        url->path.ptr[j] = 0x0;

    }

    if(scheme_end){
        url->protocol.len = (size_t)scheme_end;
        url->protocol.ptr = xmalloc( url->protocol.len+1 );


        for(i=0; i < scheme_end; i++){
            url->protocol.ptr[i] = str[i];
        }

        url->protocol.ptr[i] = 0x0;

    }


    url->hostname.len = (size_t) (host_end - host_start);
    url->hostname.ptr = xmalloc( url->hostname.len + 1 );

    for(i=host_start, j=0; i < host_end; i++, j++){
        url->hostname.ptr[j] = str[i];
    }

    url->hostname.ptr[j] = 0x0;


    return 1;
}


int main(int argc, char *argv[]){
    url_t myurl = DEFAULT_URL_T;
    url_parser_err errcode;

    if(argc != 2){ return 0; }

    if( url_parser(&myurl, argv[1], &errcode) ){

        printf("protocol: %s\n", myurl.protocol.ptr );
        printf("hostname: %s\n", myurl.hostname.ptr );
        printf("path: %s\n", myurl.path.ptr );
        printf("port: %d\n", myurl.port);

        free_str_t(myurl.protocol);
        free_str_t(myurl.hostname);
        free_str_t(myurl.path);

    } else {
        printf("Invalid url - %s\n", url_parser_strerror(errcode) );
    }

    return 0;
}

