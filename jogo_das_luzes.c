// Coded by mmxm :D
// Demo: http://i.imgur.com/P6c1IiL.gif

#include <stdio.h>

#define LUZ "\e[42;5;1;37m"
#define RESET "\e[0m"

int check_win(char x[5][5]){
	int i = 0, j = 0, ret = 1;

	for(i=0; i<5; i++){
		for(j=0; j<5; j++){
			if(!x[i][j]){
				ret = 0;
				goto end;
			}
		}
	}

	end:

	return ret;
}

void draw(char x[5][5]){
	int i, j;
	printf("\n y^\n");
	printf("    ______ ______ ______ ______ ______\n");
	for(i=4; i>=0; i--){
		printf(" %d |",i);
		for(j=0; j<5; j++){
			if(x[j][i]){
				printf("%s_0000_%s|", LUZ, RESET);
			} else {
				printf("______|");
			}
		}
		printf("\n");
	}

	printf("\n      0      1      2      3      4     x ->\n\n");


}

int getpos(char c){
	int ret = 0;
	do {
		printf("\nEnter position %c: ",c);
		ret = getchar();
		while(getchar() != '\n');
	}
	while(ret < '0' || ret > '4');

	return ret-'0';
}


void update(char k[5][5], int x, int y){

	k[x][y] = ~k[x][y];

	if(x - 1 >= 0){
		k[x-1][y] = ~k[x-1][y];
	}

	if(x + 1 <= 4){
		k[x+1][y] = ~k[x+1][y];
	}

	if(y + 1 <= 4){
		k[x][y+1] = ~k[x][y+1];
	}

	if(y - 1 >= 0){
		k[x][y-1] = ~k[x][y-1];
	}


}

int main(){
	static char k[5][5];
	int numx = 0, numy = 0;


	while(1){
		printf("\e[1;1H\e[2J");
		draw(k);

		if(check_win(k)){
			break;
		}

		numx = getpos('x');
		numy = getpos('y');

		update(k, numx, numy);
	}

	printf("You win :D\n");
	return 0;

}
