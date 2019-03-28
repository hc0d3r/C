#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define CYAN "\e[1;36m"
#define RESET "\e[0m"

#define error(f, x...) \
	fprintf(f, "%s[-]%s ", RED, RESET); \
	fprintf(f, x);

#define good(f, x...) \
	fprintf(f, "%s[+]%s ", GREEN, RESET); \
	fprintf(f, x);

#define info(f, x...) \
	fprintf(f, "%s[*]%s ", CYAN, RESET); \
	fprintf(f, x);

void chomp(char *str){
	size_t i;

	for(i=0; str[i]; i++){
		if(str[i] == '\n'){
			str[i] = 0x0;
			break;
		}
	}
}

void check_host(const char *host){

	struct addrinfo hints, *res, *addr;
	char str_ip[INET6_ADDRSTRLEN];

	memset(&hints, 0x0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;



	if( getaddrinfo(host, NULL, &hints, &res) ){
		error(stdout, "Unknow host\n");
		return;
	}

	good(stdout, "%s\n", host);

	for(addr=res; addr!=NULL; addr=addr->ai_next){
		if(addr->ai_family == AF_INET){
			inet_ntop(addr->ai_family, &((struct sockaddr_in *) addr->ai_addr)->sin_addr ,str_ip, INET6_ADDRSTRLEN);
		} else {
			inet_ntop(addr->ai_family, &((struct sockaddr_in6 *) addr->ai_addr)->sin6_addr ,str_ip, INET6_ADDRSTRLEN);
		}
		info(stdout,"%s\n", str_ip);
	}

	freeaddrinfo(res);

}

void help(void){
	good(stdout, "Subdomain finder by m\n");
	info(stdout, "Usage: ./subdomain [target-hostname] [subdomain-wordlist]\n");
	exit(0);
}

int main(int argc, char *argv[]){

	if(argc != 3){
		help();
	}

	char *target = argv[1], *arquivo = argv[2], *subdominio, line[1024];
	FILE *arq;

	int i, j;
	size_t host_len;

	if( (arq = fopen(arquivo, "rt")) == NULL ){
		error(stderr, "fopen() failed\n");
		return 1;
	}

	host_len = strlen(target);

	while(!feof(arq)){

		if( fgets(line, 1024, arq) ){
			chomp(line);

			if( (subdominio = malloc( host_len + strlen(line) + 2)) == NULL ){
				error(stderr, "malloc() failed\n");
				exit(1);
			}

			for(j=0; line[j]; j++){
				subdominio[j] = line[j];
			}

			if(j){
				if(line[j-1] != '.' && target[0] != '.'){
					subdominio[j++] = '.';
				}
			}

			for(i=0; target[i]; i++, j++){
				subdominio[j] = target[i];
			}

			subdominio[j] = 0x0;


			info(stdout, "Checking -> %s\n", subdominio);
			check_host(subdominio);
			free(subdominio);

		}
	}

	fclose(arq);

	return 0;
}
