// gcc mysql-hashcrack.c -o mysql-hashcrack -lcrypto -O2
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char hexdecode(char c){
    if(c >= '0' && c <= '9')
        c -= '0';

    else if(c >= 'a' && c <= 'f')
        c = c-'a'+10;

    else if(c >= 'A' && c <= 'F')
        c = c-'A'+10;

    else {
        printf("invalid char\n");
        exit(1);
    }

    return c;
}

int converthash(const char *str, unsigned char *out){
    int i, ret = 1;

    if(strlen(str) != SHA_DIGEST_LENGTH*2){
        goto end;
    }

    for(i=0; i<SHA_DIGEST_LENGTH*2; i+=2){
        *out++ = hexdecode(str[i]) << 4 | hexdecode(str[i+1]);
    }

    ret = 0;

    end:
    return ret;
}

int main(int argc, char **argv){
    unsigned char target[SHA_DIGEST_LENGTH],
     sha1[SHA_DIGEST_LENGTH], hash[SHA_DIGEST_LENGTH],
     *line;

    FILE *fh;

    register int i;
    size_t len, count;
    ssize_t n;

    if(argc < 3){
        printf("mysql-hashcrack [hash] [wordlist]\n");
        return 0;
    }

    line = NULL;
    count = len = 0;

    if((fh = fopen(argv[2], "r")) == NULL){
        perror("fopen()");
        return 1;
    }

    if(converthash(argv[1], target)){
        printf("invalid hash\n");
        return 1;
    }

    printf("[*] running ...\n");
    while((n = getline((char **)&line, &len, fh)) > 0){
        if(n == 1)
            continue;

        // sha1(sha1(word))
        SHA1(line, n-1, sha1);
        SHA1(sha1, SHA_DIGEST_LENGTH, hash);

        // compare
        for(i=0; i<SHA_DIGEST_LENGTH; i++){
            if(hash[i] != target[i])
                break;
        }

        if(SHA_DIGEST_LENGTH == i){
            line[n-1] = 0x0;
            printf("\n[+] hash => '%s'", line);
            break;
        }

        count++;
        printf("[*] %zu tries\r", count);
    }

    printf("\n[*] finish\n");

    return 0;
}
