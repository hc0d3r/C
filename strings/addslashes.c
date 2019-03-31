// gcc addslashes.c -o addslashes -D_HAVE_MAIN
// addslashes like PHP

#include <stdio.h>
#include <stdlib.h>

#define quote(c) (c == '"' || c == '\'' || c == '\\' || c == 0x0)

char *addslashes(const char *str, size_t size){
    char *ret, *ptr, c;
    size_t i, len = 0;

    for(i=0; i<size; i++){
        if(quote(str[i]))
            len++;
    }

    len += i+1;

    ret = ptr = malloc(len);
    if(ret == NULL)
        goto end;

    i = 0;

    while(i < size){
        c = str[i++];

        if(quote(c)){
            *ptr++ = '\\';
            if(c){
                *ptr++ = c;
            } else {
                *ptr++ = '0';
            }
        } else {
            *ptr++ = c;
        }
    }

    *ptr = 0x0;

    end:
    return ret;
}

#ifdef _HAVE_MAIN
int main(void){
    char *test = NULL;

    test = addslashes("\x00\x00\x00\x61", 4);
    printf("%s\n", test);
    free(test);

    test = addslashes("'-'", 3);
    printf("%s\n", test);
    free(test);

    // +1 for get null byte
    test = addslashes("\\ \"hello world\" \\", 18);
    printf("%s\n", test);
    free(test);


    return 0;
}
#endif
