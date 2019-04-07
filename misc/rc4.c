#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

typedef struct rc4 {
    unsigned char s[256];
    int i;
    int j;
} rc4_t;

void rc4_ksa(rc4_t *rc4, const char *key, size_t keysize){
    int i, j = 0;
    char tmp;

    rc4->i = 0;
    rc4->j = 0;

    for(i=0; i<256; i++){
        rc4->s[i] = i;
    }

    for(i=0; i<256; i++){
        j = (j + rc4->s[i] + key[i % keysize]) & 0xff;
        tmp = rc4->s[i];
        rc4->s[i] = rc4->s[j];
        rc4->s[j] = tmp;
    }

}

void rc4_prga(rc4_t *rc4, char *out, const char *data, size_t len){
    register size_t aux;
    char tmp;

    int i = rc4->i;
    int j = rc4->j;

    for(aux=0; aux<len; aux++){
        i = (i + 1) & 0xff;
        j = (j + rc4->s[i]) & 0xff;

        tmp = rc4->s[i];
        rc4->s[i] = rc4->s[j];
        rc4->s[j] = tmp;

        out[aux] = rc4->s[(rc4->s[i]+rc4->s[j]) & 0xff]^data[aux];
    }

    rc4->i = i;
    rc4->j = j;
}

int main(int argc, char **argv){
    char enc[1024], buf[1024];
    rc4_t rc4;

    size_t keysize;
    ssize_t n;
    int fd;

    if(argc != 3){
        printf("rc4 [key] [file] > output\n");
        return 0;
    }

    fd = open(argv[2], O_RDONLY);
    if(fd == -1){
        perror("open()");
        return 1;
    }

    keysize = strlen(argv[1]);
    if(!keysize){
        printf("invalid key\n");
        return 1;
    }

    rc4_ksa(&rc4, argv[1], keysize);

    while((n = read(fd, buf, sizeof(buf))) > 0){
        rc4_prga(&rc4, enc, buf, (size_t)n);
        write(STDOUT_FILENO, enc, n);
    }

    close(fd);

    return 0;
}
