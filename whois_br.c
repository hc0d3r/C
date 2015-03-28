/* ==============================
 * $ date
 * Sáb Mar 28 17:10:06 BRT 2015
 * $ whoami
 * MMxM
 * $ pwd
 * http://hc0der.blogspot.com
 * ============================== */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define WHOIS "registro.br"

void whois_br(const char *hostname);
void error(char *msg);
void help(void);

void help(void){
	fprintf(stderr,"\n[+] Whois query .br domains\n");
	fprintf(stderr,"[-] *(Coder)-> MMxM\n");
	fprintf(stderr,"[*] Usage: ./whois_br (domain).br\n");
	fprintf(stderr,"[*] Example: ./whois_br google.com.br\n\n");
	exit(1);
}

void error(char *msg){
	perror(msg);
	exit(1);
}

void whois_br(const char *hostname){
	int msocket,recebidos;
	char resposta[5000];
	struct sockaddr_in addr;
	struct hostent *host;

	msocket = socket(AF_INET,SOCK_STREAM,0);

	if(msocket == -1)
		error("Fail to create socket ");

	host = gethostbyname(WHOIS);

	addr.sin_family	= host->h_addrtype;
	addr.sin_port	= htons(43);
	addr.sin_addr	= *((struct in_addr *)host->h_addr);

	memset(&addr.sin_zero,0,sizeof(addr.sin_zero));

	if(connect(msocket,(struct sockaddr*)&addr,sizeof(addr)) == -1)
		error("Fail to connect");

	if( (send(msocket, hostname, strlen(hostname),0) == -1) || (send(msocket, "\n", 1, 0) == -1) )
		error("Fail to make request");

	while( (recebidos = recv(msocket,resposta,4999,0)) ){
		resposta[recebidos] = '\0';
		puts(resposta);
	}
}

int main(int argc,char **argv){

	if(argc != 2)
		help();

	whois_br(argv[1]);

	return 0;
}
