#include <stdio.h>
#include <unistd.h>

/* ************************************ *
 * CPU USAGE CALC , Using /proc/stat	*
 * Sáb Mar 28 16:18:44 BRT 2015		*
 * Author: MMxM | hc0der.blogspot.com	*
 * ************************************	*/



int main(void){
	FILE *proc;

	char c;

	int i=0, cpu_found=0, tmp = 0, total=0, prev_total=0, diff_idle=0, diff_total=0, idle=0, prev_idle=0, x=0, diff_usage=0;

	while(1){
		if( (proc = fopen("/proc/stat","r")) == NULL){
			printf("Failed to open file\n");
			return 1;
		}


		while( (c = fgetc(proc)) != EOF ){
			if( (c == 'c' && i == 0) || (c == 'p' && i == 1) || (c == 'u' && i == 2)){
				cpu_found++;
			}

			else if( c >= '0' && c <= '9' && cpu_found == 3){
				tmp *= 10;
				tmp += c-'0';
			}

			else if( c == ' ' && cpu_found == 3 ){
				total += tmp;
				x++;
				if( x == 6 )
					idle = tmp;
				tmp = 0;
			}

			else if( c == '\n' && cpu_found == 3 ){
				i = 0;
				cpu_found = 0;
				total += tmp;
				tmp = 0;
				break;
			}
			i++;
		}

		diff_idle = idle-prev_idle;
		diff_total = total-prev_total;

		prev_total=total;
		prev_idle=idle;

		diff_usage=100*(diff_total - diff_idle);

		printf("CPU: %6.2f%%\r", (float)diff_usage/(float)diff_total );
		fflush(stdout);
		sleep(1);
	}

	return 0;

}
