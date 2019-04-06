#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#define LUZ "\e[42;5;1;37m"
#define RED "\e[41m"
#define RESET "\e[0m"

struct termios oldt;

void init_terminal(void){
    struct termios newt;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ICANON|ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restore_terminal(void){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int win(char vet[5][5]){
    int ret = 1, x, y;

    for(x=0; x<5; x++){
        for(y=0; y<5; y++){
            if(!vet[x][y]){
                ret = 0;
                goto end;
            }
        }
    }

    end:

    return ret;
}


void draw(char vet[5][5], int px, int py){
    char *fmt;
    int x, y;

    printf("  ______ ______ ______ ______ ______\n");
    for(y=4; y>=0; y--){
        printf(" |");
        for(x=0; x<5; x++){
            if(px == x && py == y){
                if(!vet[x][y]){
                    fmt = "__on__";
                } else {
                    fmt = RED "_off__" RESET;
                }
            } else {
                if(vet[x][y]){
                    fmt = LUZ "_0000_" RESET;
                } else {
                    fmt = "______";
                }
            }

            printf("%s|", fmt);
        }
        printf("\n");
    }

}

int getmoviment(int *px, int *py){
    int ret = 0;
    char c[3];
    ssize_t n;

    int x, y;

    x = *px;
    y = *py;

    n = read(STDIN_FILENO, c, sizeof(c));

    if(n == 3 && c[0] == 27 && c[1] == 91){
        // up
        if(c[2] == 65 && y+1 < 5){
            y++;
        }

        // down
        if(c[2] == 66 && y-1 >= 0){
            y--;
        }

        // right
        if(c[2] == 67 && x+1 < 5){
            x++;
        }

        // left
        if(c[2] == 68 && x-1 >= 0){
            x--;
        }
    }

    if(n <= 0)
        ret = 'q';
    else
        ret = c[0];

    *px = x;
    *py = y;

    return ret;
}


void update(char vet[5][5], int x, int y){

    vet[x][y] = ~vet[x][y];

    if(x - 1 >= 0){
        vet[x-1][y] = ~vet[x-1][y];
    }

    if(x + 1 <= 4){
        vet[x+1][y] = ~vet[x+1][y];
    }

    if(y + 1 <= 4){
        vet[x][y+1] = ~vet[x][y+1];
    }

    if(y - 1 >= 0){
        vet[x][y-1] = ~vet[x][y-1];
    }

}

int main(void){
    char vet[5][5];
    int x, y;

    memset(vet, 0x0, sizeof(vet));
    x = y = 2;

    printf("press q to exit\n");

    init_terminal();

    do {
        draw(vet, x, y);
        char c = getmoviment(&x, &y);
        if(c == 'q'){
            goto end;
        } else if(c == 32){
            update(vet, x, y);
        }

        printf("\e[6A");

    } while(!win(vet));

    draw(vet, 5, 5);
    printf("\nyou win !\n");

    end:
    restore_terminal();

    return 0;
}
