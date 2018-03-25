#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int *parser_result(const char *buf, int size){
	static int ret[10];
	int i, j = 0, start = 0;

	for(i=0; i<size; i++){
		char c = buf[i];
		if(c >= '0' && c <= '9'){
			if(!start){
				start = 1;
				ret[j] = c-'0';
			} else {
				ret[j] *= 10;
				ret[j] += c-'0';
			}
		} else if(c == '\n'){
			break;
		} else {
			if(start){
				j++;
				start = 0;
			}
		}
	}

	return ret;
}



int main(int argc, char **argv){
	char buf[256];
	int size, fd, *nums, prev_idle = 0, prev_total = 0, idle, total, i;

	fd = open("/proc/stat", O_RDONLY);

	while(1){
		size = read(fd, buf, sizeof(buf));
		if(size <= 0)
			break;

		nums = parser_result(buf, size);

		idle=nums[3];

		for(i=0, total=0; i<10; i++){
			total += nums[i];
		}


		int diff_idle = idle-prev_idle;
		int diff_total = total-prev_total;
		float usage = (float)(((float)(1000*(diff_total-diff_idle))/(float)diff_total+5)/(float)10);
		printf("\r%%%6.2f", usage);
		fflush(stdout);

		prev_total = total;
		prev_idle = idle;

		sleep(3);
		lseek(fd, 0, SEEK_SET);
	}

	return 0;
}
