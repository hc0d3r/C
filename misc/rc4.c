// RC4 Exemplo, coded by mmxm

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define swap(x,y) x ^= y; y ^= x; x ^= y

typedef unsigned char uchar;

struct dynamic_str {
	uchar *ptr;
	size_t len;
};

void ksa(uchar *s, uchar *key, size_t key_size){
	int i=0,j=0;

	for(i=0; i<256; i++){
		s[i]=i;
	}

	for(i=0; i<256; i++){
		j = (j+s[i]+ key[i % key_size]) % 256;
		swap(s[i], s[j]);
	}

}

void prga(const uchar *word, uchar *s, struct dynamic_str *result){
	size_t i=0,j=0, aux;

	result->ptr = malloc(1);
	result->len = 1;


	for(aux=0; word[aux]; aux++){
		i = (i+1)%256;
		j = (j+s[i])%256;
		swap(s[i], s[j]);

		result->len++;
		result->ptr = realloc(result->ptr, result->len);
		result->ptr[aux] = (s[(s[i]+s[j]) %256]^word[aux]);
	}

	result->ptr[aux] = 0x0;
}

int main(void){
	uchar s[256];
	uchar key[256];
	uchar word[256];
	size_t key_len;

	struct dynamic_str encode, decode;

	strcpy((char *)key, "do_or_die"); // > 255 == stack overflow
	strcpy((char *)word, "\natirei o pau na dilma, mas a dilma não morreu\n");

	key_len = strlen((const char *)key);

	ksa(s, key, key_len);
	prga(word, s, &encode);

	printf("RC4:\n'%s'\n", encode.ptr);

	ksa(s, key, key_len);
	prga(encode.ptr, s, &decode);

	printf("Decoded:\n'%s'\n", decode.ptr);

	free(decode.ptr);
	free(encode.ptr);

	return 0;
}
