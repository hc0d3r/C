/* ==============================
 * $ date
 * Wed Dec 25 14:51:43 BRST 2013
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

char *whois_br(char *hostname);
int error(char *msg);
int help();

int help(){
	fprintf(stderr,"\n[+] Whois query .br domains\n");
	fprintf(stderr,"[-] *(Coder)-> MMxM\n");
	fprintf(stderr,"[*] Usage: ./whois_br (domain).br\n");
	fprintf(stderr,"[*] Example: ./whois_br google.com.br\n\n");
	exit(1);
}

int error(char *msg){
	perror(msg);
	exit(1);
}

char *whois_br(char *site_name){
	int msocket,recebidos;
	char *resposta = (char *)malloc(5000);
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

	strncat(site_name,"\n",1);

	if(send(msocket,site_name,strlen(site_name),0) == -1)
		error("Fail to make request");

	while(recebidos = recv(msocket,resposta,4999,0))
		resposta[recebidos] = '\0';


	return resposta;
}

int main(int argc,char **argv){

	if(argc != 2)
		help();

	char *whois = whois_br(argv[1]);
	fprintf(stdout,"%s",whois);
	free(whois);
}
