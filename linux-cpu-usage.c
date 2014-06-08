#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* ************************************ *
 * CPU USAGE CALC , Using /proc/stat	*
 * Qui Mai  1 11:26:43 BRT 2014		*
 * Author: MMxM | hc0der.blogspot.com	*
 * ************************************	*/

int main(void){
	FILE *proc;
	char line[60];
	int prev_idle = 0,
	prev_total = 0, idle = 0,diff_usage = 0,
	diff_idle = 0,diff_total = 0,
	tmp_p = 0, v_p = 0,sum = 0;
	char tmp[10];
	while(1){
		int vet[10] = { 0 },i=0;
		v_p = 0,sum = 0;
		proc = fopen("/proc/stat","r");
		fgets(line,60,proc);
		int len = strlen(line)-1;
		line[len] = '\0';
		while(i!=len-1){
			if(line[i] >= 48 && line[i] <= 57){
				while(i){
					if(line[i] == 32 || line[i] == 10) break;
					tmp[tmp_p] = line[i];
					i++;
					tmp_p++;
				}
				tmp[tmp_p] = '\0';
				vet[v_p] = atoi(tmp);
				v_p++;
				tmp_p = 0;
			}
			i++;
		}

		int ii;
		for(ii=0;ii<9;ii++)
			sum += vet[ii];

		idle = vet[3];
		diff_idle = idle - prev_idle;
		diff_total = sum - prev_total;
		diff_usage = 100*(diff_total - diff_idle);
		prev_idle = idle;
		prev_total = sum;
		fflush(stdout);
		fprintf(stdout,"CPU: %6.2f%%\r",(float)diff_usage/(float)diff_total);
		
		fclose(proc);
		sleep(1);
	}


}
