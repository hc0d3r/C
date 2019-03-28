#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <assert.h>
#define PROC "/proc/"

/* ************************************ *
 * Get PIDs and cmdline from /proc/	*
 * Ter Fev 10 18:38:31 BRST 2015	*
 * Author: MMxM | hc0der.blogspot.com	*
 * ************************************ */


void *xmalloc(size_t len);
void safefree(void **ptr);
void die(const char *err);
int cmdline_info(const char *cmd);
void status_info(const char *path, size_t len);

void die(const char *err){
	(errno) ? perror(err) : fprintf(stderr,"%s\n",err);
	exit(1);
}

void *xmalloc(size_t len){
	void *ptr = malloc(len);
	if(ptr == NULL)
		die("malloc error");
	return ptr;
}

void xfree(void **ptr){
	assert(ptr);
	if(ptr != NULL){
		free(*ptr);
		*ptr = NULL;
	}
}

int cmdline_info(const char *cmd){
	FILE *fp = NULL;
	char C = 0;
	size_t x = 0;


	if( (fp = fopen(cmd,"r")) == NULL)
		return 0;

	while( (C = fgetc(fp)) != EOF )
		x += printf("%c",C);

	fclose(fp);

	return x;
}

void status_info(const char *path, size_t len){
	char *status_file = NULL, C = 0;
	FILE *fp = NULL;

	status_file = strncpy( xmalloc( (len+6) * sizeof(char) ), path, len);
	strncat(status_file, "status", 6);

	if( (fp = fopen(status_file, "r")) != NULL){
		fseek(fp, 6, SEEK_SET);
		while( (C = fgetc(fp)) != '\n')
			printf("%c",C);

		fclose(fp);
	}

	xfree((void **)&status_file);

}

int main(void){
	DIR *dip;
	struct dirent *dit;
	struct stat owner;
	struct passwd *pd;
	int pid_nb = 0;

	char *file = NULL, *cmdline = NULL;
	size_t alloc_size = 0;

	if( (dip=opendir(PROC)) == NULL )
		die("opendir() error");


	printf("USER\tPID\tCOMMAND\n");

	while ( (dit = readdir(dip) ) != NULL ){
		if(dit->d_type != 4)
			continue;

		pid_nb = (int) strtol(dit->d_name, NULL, 10);

		if(pid_nb == 0)
			continue;

		alloc_size = 6+strlen(dit->d_name)+2;

		file = xmalloc( alloc_size * sizeof(char));
		snprintf(file, alloc_size, "%s%s/",PROC,dit->d_name);

		if(stat(file, &owner) == -1){
			xfree((void **)&file);
			continue;
		}

		if((pd = getpwuid(owner.st_uid)) == NULL){
			xfree((void **)&file);
			continue;
		}

		printf("%s\t",pd->pw_name);
		printf("%s\t",dit->d_name);

		cmdline = strncpy( xmalloc( alloc_size + 7 ) , file , alloc_size );
		strncat(cmdline,"cmdline",7);

		if(!cmdline_info(cmdline)){
			printf("[");
			status_info(file, alloc_size);
			printf("]");
		}

		printf("\n");

		xfree((void **)&cmdline);
		xfree((void **)&file);
	}

	closedir(dip);

	return 0;

}
