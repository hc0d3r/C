// gcc bing_dorker.c -o bing-search -lcurl

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#define URI "www.bing.com/search?q=%s&first=%d&FORM=PERE"

struct request_data {
    char *ptr;
    size_t len;
    size_t index;
};

void die(const char *err){
    perror(err);
    exit(1);
}

void help(void){
    const char *banner =
        "usage: bing-search [OPTIONS] [SEARCH STRING]\n\n"
        "Options:\n"
        " -q          quiet mode\n"
        " -o FILE     write result to file";

    puts(banner);
    exit(0);
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct request_data *s){
    size_t new_len = s->index + size*nmemb;

    // I use s->len for not need free and alloc/realloc
    // the memory every time, instead of this I only set s->index to 0
    if(new_len > s->len){
        s->len = new_len;
        s->ptr = realloc(s->ptr, new_len+1);
    }

    if(s->ptr == NULL)
        die("realloc() failed");

    memcpy(s->ptr+s->index, ptr, size*nmemb);
    //s->ptr[new_len] = '\0';
    s->index = new_len;

    return size*nmemb;
}

int extract_link(struct request_data *body, int verbose, FILE *output){
    static const char tag[]="<li class=\"b_algo\"><h2><a href=\"";

    char *aux, *ptr = body->ptr;
    int ret = 0;

    while((ptr = strstr(ptr, tag))){
        // -1 for null byte
        ptr += sizeof(tag)-1;

        if((aux = strchr(ptr, '"')) == NULL)
            break;

        *aux = 0x0;

        if(verbose)
            puts(ptr);

        if(output)
            fprintf(output, "%s\n", ptr);

        ptr = aux+1;
        ret++;
    }

    return ret;
}

int main(int argc, char **argv){
    char *filename = NULL, *search = NULL;
    int verbose = 1, opt, i;

    struct request_data body;
    char *url, *encode;

    FILE *output = NULL;
    CURL *curl;

    size_t len;

    while((opt = getopt(argc, argv, "qo:")) != -1){
        switch(opt){
            case 'q':
                verbose = 0;
                break;
            case 'o':
                filename = optarg;
                break;
            default:
                return 1;
        }
    }

    search = argv[optind];

    if(!search || (!filename && !verbose))
        help();

    if(filename){
        if((output = fopen(filename, "a")) == NULL)
            die("fopen()");
    }

    printf("searching '%s' ...\n\n", search);

    // escape string
    encode = curl_easy_escape(NULL, search, strlen(search));

    // encoded string + URI
    len = strlen(encode)+56;

    if((url = malloc(len)) == NULL){
        die("malloc() failed");
    }

    // init curl and setopts
    body.ptr = NULL;
    body.len = 0;

    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    for(i=1; i<=211; i+=10){
        // set index to 0
        body.index = 0;

        // I dont use snprintf here, because I set many space for url
        // but if you change anything keep an eye on it
        sprintf(url, URI, encode, i);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        if(curl_easy_perform(curl) != CURLE_OK)
            break;

        // setting null byte
        if(body.ptr)
            body.ptr[body.index] = 0;

        if(!extract_link(&body, verbose, output)){
            break;
        }
    }

    curl_easy_cleanup(curl);
    curl_free(encode);
    free(body.ptr);
    free(url);

    if(output)
        fclose(output);

    printf("\n[ Finish !!! ]\n");

    return 0;
}
