#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// By MMxM
// C version of addslashes (PHP)


char *addslashes(const char *str);

char *addslashes(const char *str){
	int i = 0,x = 0, j = 0,size = strlen(str),size_x;
	char *str2;

	for(i=0;i<size;i++){
		if(str[i] == 0x5c)
			x++;
		else if(str[i] == 0x27)
			x++;
		else if(str[i] == 0x22)
			x++;
	}

	size_x = x+i;

	str2 = (char *)malloc(size_x);
	memset(str2,0,sizeof(str2));

	for(i=0;i<size;i++){
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
	return str2;
}

int main(void){
	const char *test = "I'm X | \"Y\" | C:\\Windows\\System32 ' ' \\' ' ";
	char *test2 = addslashes(test);
	printf("Without addslashes: %s\n",test);
	printf("With addslashes: %s\n",test2);
	free(test2);
	return 0;
}
