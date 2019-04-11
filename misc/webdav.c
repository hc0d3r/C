// gcc webdav.c -lcurl webdav

#include <sys/stat.h>

#include <curl/curl.h>
#include <stdio.h>

int main(int argc, char **argv){
    struct stat st;
    int ret = 1;
    FILE *fh;
    CURL *ch;

    if(argc != 3){
        printf("webdav [target-url] [file]\n");
        ret = 0;
        goto end;
    }

    fh = fopen(argv[2], "r");
    if(fh == NULL){
        perror("fopen()");
        goto end;
    }

    if(fstat(fileno(fh), &st) == -1){
        perror("fstat()");
        goto end;
    }

    ch = curl_easy_init();
    if(ch == NULL){
        perror("curl_easy_init()");
        goto end;
    }

    curl_easy_setopt(ch, CURLOPT_URL, argv[1]);
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(ch, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(ch, CURLOPT_READDATA, fh);
    curl_easy_setopt(ch, CURLOPT_INFILESIZE_LARGE, st.st_size);
    curl_easy_setopt(ch, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(ch, CURLOPT_VERBOSE, 1L);

    curl_easy_perform(ch);


    fclose(fh);
    curl_easy_cleanup(ch);


    end:
    return ret;
}
