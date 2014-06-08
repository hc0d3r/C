#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#define PROC "/proc/"

/* ************************************ *
 * Get PIDs and cmdline from /proc/	*
 * Qui Mai  1 14:01:54 BRT 2014		*
 * Author: MMxM | hc0der.blogspot.com	*
 * ************************************ */


int main(){
		DIR *dip;
		struct dirent *dit;
		struct stat owner;
		struct passwd pd;
		struct passwd* pwdptr=&pd;
		struct passwd* tempPwdPtr;
		char pwdbuffer[200];
		int  pwdlinelen = sizeof(pwdbuffer);
		FILE *info;
		int name_s;
		
		if( (dip=opendir(PROC)) == NULL ){
				perror("opendir");
				return(1);
		}
		printf("USER\tPID\tCOMMAND\n");
		while ( (dit = readdir(dip) ) != NULL ){
			char name[500];
			
			if(dit->d_type!=4)
				continue;
				
			if(atoi(dit->d_name)==0)
				continue;
			int numero = strlen(PROC)+strlen(dit->d_name)+2;
			char f[numero];
			snprintf(f,numero,"%s%s/",PROC,dit->d_name);
			if(stat(f, &owner) == -1)
				continue;
			
			f[0] = '\0';
			int id = owner.st_uid;
			getpwuid_r(id,pwdptr,pwdbuffer,pwdlinelen,&tempPwdPtr);
			printf("%s\t",pd.pw_name);
			
			
			int dir_size = strlen(dit->d_name)+6+2+7;
			printf("%s\t",dit->d_name);
			char cmd_line[dir_size];
			snprintf(cmd_line,dir_size,"%s%s/cmdline",PROC,dit->d_name);
			info = fopen(cmd_line,"r");
			
			fgets(name,500,info);
			name_s = strlen(name);
			if(name_s > 0){
				printf("%s\n",name);
				fclose(info);
			} else {
				fclose(info);
				int dir_s = strlen(dit->d_name)+6+2+6;
				char status[dir_s];
				snprintf(status,dir_s,"%s%s/status",PROC,dit->d_name);
				info = fopen(status,"r");
				fgets(name,500,info);
				int len = strlen(name);
				int i;
				printf("[");
				name[len-1] = ']';
				for(i=6;i<len;i++)
					printf("%c",name[i]);
				printf("\n");
			}
			name[0]='\0';
			
		}
		return(0);

}
