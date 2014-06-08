#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

/*

%# date
Wed Mar  5 00:38:56 BRT 2014
%# hostname
hc0der.blogspot.com
%# cat readme
                            _.
                         ,-" .\,-"`.		 _______________
                       ,:   . /,-.  `.		( Coded by MMxM )
                      /"     :,-  `   \		 ----------------
                     |.    .`/,. `.    \       /
                    /  /  /_\)/_\       .     /
                   .  .  /' / \ "\	     /
                   |  : ,' .   . \ \\ . |   /
                   |  : || |   | | || . '
                   \__\_bo..  ...bo)L L/
                     |9|\_*_" "_*_/|?T
                     \(` --- L --- ')/
                    .'`-|   ___   |-:
               .---"  -. .   =   ,   `.
               ""-._ --./ "-._.-"|     "-._
                  .-"-. \  \      \   _..-<_
              .--"     ) `. \   /  | :-"..  \
           .-'_     \ ,'  \\"`.,'""   \   `  |
          /    `.    | "-. `   \    |/"    | \
              .  \      _ \:|   |  ,|."""""| /
         /   :    |.-""" | \|`._'_.  |`.  .( \
        /|  '     ||_  .' `:'        |: `v \  |
       / | :      |  "")  :|         |:  |  | |
       |          /|   /  :|         |:  /  / )
       (         | /_.|...:|         /  |  |  |
      / \        /_..-----"'         """-,-   |
     |          /`--\                   |\  `.)
     |         |     |                ,/ `. `.\
     /   _Seal_|      "._..---""""---./\ ` `. )


*/

char *chomp(char *sub){
	int len = strlen(sub)-1;
	if(sub[len] == '\n')
		sub[len] = '\0';
	return sub;
}

void check_host(char *host){
	struct hostent *hostname;
	struct in_addr **address_list;
	int i;

	hostname = gethostbyname(host);
	if(hostname == NULL){
		fprintf(stdout,"%s[-]%s Unknown host\n",RED,RESET);
		return;
	}

	address_list = (struct in_addr **) hostname->h_addr_list;

	fprintf(stdout,"%s[+]%s %s { ",GREEN,RESET,host);
	for(i=0;address_list[i] != NULL; i++)
		fprintf(stdout,"'%s' ",inet_ntoa(*address_list[i]));
	fprintf(stdout,"}\n");
}

void help(){
	fprintf(stderr,"\n[%s+%s] Subdomain Finder by MMxM\n",GREEN,RESET);
	fprintf(stderr,"[%s*%s] Usage: ./sub <target> <subdomain_wordlist>\n\n",CYAN,RESET);
	exit(1);
}

int main(int argc, char *argv[]){
	if(argc != 3)
		help();

	const char *target = argv[1];
	const char *arquivo = argv[2];

	char line[100];
	char *result;
	FILE *arq;

	if( (arq = fopen(arquivo, "rt")) == NULL ){
		fprintf(stdout,"%s[-]%s fopen(\"%s\",\"rt\"): %s \n",RED,RESET,arquivo,strerror(errno));
		return 1;
	}

	while(!feof(arq)){
		result = fgets(line,100,arq);
		if(result){
			chomp(result);
			int size = strlen(result)+strlen(target)+2;
			char *subdominio = (char *) malloc(size);
			sprintf(subdominio,"%s.%s",result,target);
			fprintf(stdout,"%s[*]%s Checking => %s\n",CYAN,RESET,subdominio);
			check_host(subdominio);
			free(subdominio);
		}
	}

	return 0;
}
