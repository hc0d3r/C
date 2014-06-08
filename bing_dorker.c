#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <getopt.h>

/*

 Bing Dorker
 By MMxM

 [hc0der.blogspot.com]

*/

int verbose = 0;
int outp = 0;
FILE *out;

void open_file(const char *outo){
	out = fopen(outo,"a");
	if(out == NULL){
		fprintf(stderr,"\nError: Open file %s\n",outo);
		exit(1);
	}
}

void help_m(){
	printf("\n Bing Dorker by MMxM\n");
	printf(" [hc0der.blogspot.com]\n\n");
	printf(" Options:\n\n");
	printf("\t-s [string to search]\n");
	printf("\t-v [be verbose]\n");
	printf("\t-o [output file]\n\n");
	printf(" Example:\n\n");
	printf("\t./bing -s 'a b c' -v -o /tmp/urls.txt\n\n");
	exit(0);
}

struct string{
	char *ptr;
	size_t len;
};

void init_string(struct string *s){
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s){
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}

	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

int extract_link(char *html){
	int count = 0;
	char search[]="<li class=\"b_algo\"><h2><a href=";
	int reg_s = strlen(search);
	int vet_p = 1;

	while(1){
		char *extrai = strstr(html,search);
		if(extrai){
			char c = html[extrai-html+reg_s+vet_p];
			if(c != '"'){
				if(verbose) printf("%c",c);
				if(outp) fputc(c,out);
				vet_p++;
			} else {
				count++;
				if(verbose) printf("\n");
				if(outp) fputc('\n',out);
				memset(extrai,0,sizeof(extrai));
				memset(html,1,(extrai-html+reg_s+vet_p+1));
				vet_p = 1;
			}
		} else {
			break;
		}
	}

	return (count > 0 ? 0 : 1);

}

char *url_encode(char *s){
        CURL *a;
        a = curl_easy_init();
        char *encode = (char*)curl_easy_escape(a, s, strlen(s));
        curl_easy_cleanup(a);
        return encode;
}


int main(int argc,char *argv[]){
	int o;
	char *output = NULL;
	char *search = NULL;

	while ((o = getopt (argc, argv, "s:o:v")) != -1)
		switch(o){
			case 'v':
				verbose = 1;
				break;
			case 's':
				search = optarg;
				break;
			case 'o':
				output = optarg;
				outp = 1;
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

	if(search == NULL)
		help_m();

	if(output == NULL && verbose == 0)
		help_m();

	if(output != NULL)
		open_file(output);

	printf("\nSearching for '%s' ...\n\n",search);
	char *encode = url_encode(search);

	int i;
	char url[strlen(encode)+sizeof(int)+39];

	for(i=1;i<501;i+=10){
		CURL *curl;
		curl = curl_easy_init();
		struct string s;
		init_string(&s);

		memset(url,0,sizeof(url));

		snprintf(url,sizeof(url),"www.bing.com/search?q=%s&first=%d&FORM=PERE",encode,i);

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:29.0) Gecko/20100101 Firefox/29.0");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		curl_easy_perform(curl);

		if(extract_link(s.ptr) != 0){
			if(output != NULL) fclose(out);
			free(s.ptr);
			curl_easy_cleanup(curl);
			break;
		}

		free(s.ptr);
		curl_easy_cleanup(curl);
	}

	curl_free(encode);
	printf("\n[ Finish !!! ]\n\n");
	return(0);
}
