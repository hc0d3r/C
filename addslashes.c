#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// By MMxM
// C version of addslashes (PHP)

char *addslashes(const char *str);
void *xmalloc(size_t len);
void safefree(void **ptr);
void die(const char *err);

void die(const char *err){
	(errno) ? perror(err) : fprintf(stderr,"%s\n",err);
	exit(1);
}

void *xmalloc(size_t len){
	void *ptr = malloc(len);
	if(ptr == NULL)
		die("malloc error");
}

void xfree(void **ptr){
	assert(ptr);
	if(ptr != NULL){
		free(*ptr);
		*ptr = NULL;
	}
}

char *addslashes(const char *str){
	size_t i = 0, x = 0, j = 0, size_x = 0;
	char *str2 = NULL;

	for(i=0; str[i]; i++){
		if(str[i] == 0x5c)
			x++;
		else if(str[i] == 0x27)
			x++;
		else if(str[i] == 0x22)
			x++;
	}

	size_x = x+i+1;

	str2 = malloc(size_x*sizeof(char));

	for(i=0;str[i];i++){
		char c = str[i];

		if(c == 0x27){
			str2[j] = 0x5c;
			j++;
			str2[j] = c;
		}

		else if(c == 0x5c){
			str2[j] = c;
			j++;
			str2[j] = c;
		}

		else if(c == 0x22){
			str2[j] = 0x5c;
			j++;
			str2[j] = c;
		}

		else {
			str2[j] = c;
		}

		j++;
	}

	str2[j] = 0x0;
	return str2;
}

int main(int argc, char **argv){
	char *test = NULL;

	if(argc != 2)
		return 0;

	test = addslashes(argv[1]);

	printf("Without addslashes: %s\n",argv[1]);
	printf("With addslashes: %s\n",test);

	xfree((void **)&test);

	return 0;
}
