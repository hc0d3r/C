#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#define SEARCH "<li class=\"b_algo\"><h2><a href="
#define URI "www.bing.com/search?q=%s&first=%d&FORM=PERE"
#define putchit(x) \
	if(verbose) \
		putchar(x); \
	if(output) \
		putc(x, output);

struct dynamic_str {
	char *ptr;
	size_t len;
};

void die(const char *err){
	perror(err);
	exit(1);
}

void help(void){
	printf("\nBing Dorker by MMxM\n");
	printf("[hc0der.blogspot.com]\n\n");
	printf("\tOptions:\n\n");
	printf("\t-s [string to search]\n");
	printf("\t-v [verbose mode]\n");
	printf("\t-o [output file]\n\n");
	exit(0);
}

void init_string(struct dynamic_str *s){
	s->len = 0;
	s->ptr = malloc(s->len+1);

	if(s->ptr == NULL)
		die("malloc() failed");

	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct dynamic_str *s){
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);

	if(s->ptr == NULL)
		die("realloc() failed");

	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

int extract_link(const char *body, int verbose, FILE *output){
	static const char search[]="<li class=\"b_algo\"><h2><a href=\"";
	int i,j,ret = 0;

	for(i=0; body[i]; i++){
		for(j=0 ; body[i+j] == search[j]; j++){
			if(search[j+1] == 0x0){
				j++;

				for(;body[i+j] && body[i+j] != '"'; j++){
					putchit(body[i+j]);
				}

				putchit('\n');

				i += j;
				ret = 1;
			}
		}
	}

	return ret;
}

int main(int argc,char *argv[]){
	char *filename=NULL, *search=NULL, *encode=NULL, o;
	struct dynamic_str body, url;
	FILE *output=NULL;
	int verbose=0, i;
	CURL *curl;

	while ((o = getopt (argc, argv, "s:o:v")) != -1)
		switch(o){
			case 'v':
				verbose = 1;
				break;
			case 's':
				search = optarg;
				break;
			case 'o':
				filename = optarg;
				break;
			case '?':
				if(optopt == 's')
					fprintf(stderr, "Option -s requires an argument.\n");
				else if(optopt == 'o')
					fprintf(stderr, "Option -o requires an argument.\n");
				else if(optopt == 0)
					break;
				else
					fprintf (stderr, "Unknown option `-%c'.%d\n", optopt,optopt);
				return 1;
			default:
				abort();
	}

	if(search == NULL || (filename == NULL && verbose == 0))
		help();

	if(filename){
		if( (output = fopen(filename, "a")) == NULL )
			die("fopen()");
	}

	printf("\nSearching for '%s' ...\n\n",search);
	encode = curl_easy_escape(NULL, search, strlen(search));

	url.len = strlen(encode)+sizeof(int)+40;

	if( (url.ptr = malloc( url.len )) == NULL ){
		die("malloc() failed");
	}


	for(i=1;i<=211;i+=10){
		curl = curl_easy_init();
		init_string(&body);

		memset(url.ptr, 0, url.len-1);
		snprintf(url.ptr, url.len-1, URI, encode, i);

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		curl_easy_perform(curl);

		if(!extract_link(body.ptr, verbose, output)){
			free(body.ptr);
			curl_easy_cleanup(curl);

			if(output)
				fclose(output);
			break;
		}

		free(body.ptr);
		curl_easy_cleanup(curl);
	}

	curl_free(encode);
	free(url.ptr);

	printf("\n[ Finish !!! ]\n\n");
	return 0;
}
