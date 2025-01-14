#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "shape.h"
#include "input.h"

typedef struct _tetrimino {
    int shape;
    int rot;
} tetrimino;

int board[22][17] = {0,};
int shapeArr[7] = {0,};
int cposX, cposY, cnt = 0, swapcnt = 0, swapshape, swaprot;
tetrimino current;
struct timeval startTime; 

void render() {
    for (int i = 0; i < 12; i++) {
        board[0][i] = 3;
        board[21][i] = 3;
    }

    for (int i = 0; i < 20; i++) {
        board[1 + i][0] = 3;
        board[1 + i][11] = 3;
    }
}

int checkblock(int x, int y, int shape, int rot) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if(tetrimino_shapes[shape][rot][i * 4 + j] == 0)
                continue;

            if (board[y + i][x + j] >= 2)
                return 0;
        }
    }
    
    return 1;
}

void addblock(int x, int y, int shape, int rot, int diff) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            board[y + i][x + j] += tetrimino_shapes[shape][rot][i * 4 + j] * diff;
        }
    }
}

int nextblock() {
    
    if (cnt == 7) {
        cnt = 0;
        memset(shapeArr, 0, sizeof(shapeArr));
    }

    int a = rand() % 7;
    while (shapeArr[a]) {
        a = rand() % 7;
    }

    current.shape = a;
    current.rot = rand() % 4;
    cposX = 4;
    cposY = 1;
    shapeArr[a]++;
    cnt++;

    if (checkblock(cposX, cposY, current.shape, current.rot) == 0) {
        return 0;
    }

    addblock(cposX, cposY, current.shape, current.rot, 1);
    return 1;
}

int move(int dx, int dy, int drot) {

    if (checkblock(cposX + dx, cposY + dy, current.shape, (current.rot + drot) % 4) == 1){       
        addblock(cposX, cposY, current.shape, current.rot, -1);
        cposX += dx;
        cposY += dy;
        current.rot = (current.rot + drot) % 4;
        addblock(cposX, cposY, current.shape, current.rot, 1);
        return 1;
    } else {
        return 0;
    }
}

void complete() {
    int arr[20] = {0,};

    for (int i = 1; i < 21; i++) {
        for (int j = 1; j < 11; j++) {
            if (board[i][j] != 2)
                break;
            
            if (j == 10){
                for (int k = 1; k < 11; k++) {
                    board[i][k] = 0;
                }
                
                arr[i] = i;
            }
        }
    }

    for (int i = 2; i < 21; i++) {
        if (i == arr[i]) {
            for (int j = i; j > 1; j--) {
                for (int k = 1; k < 11; k++) {
                    board[j][k] = board[j - 1][k];
                }
            }
        }
    }
}

int harddrop() {
    while (move(0, 1, 0));
    addblock(cposX, cposY, current.shape, current.rot, 1);
    complete();
    return nextblock();
}

int swap (int x, int y, int shape, int rot) {
    int tempa, tempb;

    if (swapcnt == 0) {
        addblock(x, y, shape, rot, -1);
        addblock(13, 1, shape, rot, 1);
        swapshape = shape;
        swaprot = rot;
        swapcnt++;
        return nextblock();
    } else {
        addblock(13, 1, swapshape, swaprot, -1);
        addblock(x, y, shape, rot, -1);
        addblock(4, 1, swapshape, swaprot, 1);
        addblock(13, 1, shape, rot, 1);
        tempa = swapshape;
        tempb = swaprot;
        swapshape = shape;
        swaprot = rot;
        cposX = 4;
        cposY = 1;
        current.shape = tempa;
        current.rot = tempb;
        return 1;
    }
}

void print() {
    printf("\x1b[H");

    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 17; j++) {
            switch (board[i][j]) {
                case 0:
                    printf(" ");
                    break;
                case 1:
                case 2:
                    printf("@");
                    break;
                case 3:
                    printf("■");
            }
        }
        printf("\n");
    }
}

void timer() {
    struct timeval endTime;
    long long diffTime;

    gettimeofday(&endTime, NULL);
    diffTime = ( endTime.tv_sec - startTime.tv_sec ) * 1000000 + (endTime.tv_usec - startTime.tv_usec);

    if (diffTime > 1000000) {
        gettimeofday(&startTime, NULL);
        if (move(0, 1, 0) == 0) {
            addblock(cposX, cposY, current.shape, current.rot, 1);
            complete();

            if (nextblock() == 0) {
                print();
                exit(0);
            }
        }
        print();
    } 
}

void input() {
    while(1) {       
        if (kbhit() == 0) {
            timer();
            continue;
        }

        int ch = getchar();
        switch (ch) {
            case 27:
                if (getchar() == 91) {
                    int b = getchar();
                    if (b == 65) {
                        move(0, 0, 1);                   
                    } else if (b == 66) { // d
                        move(0, 1, 0);
                        gettimeofday(&startTime, NULL);
                    } else if (b == 67) { // r
                        move(1, 0, 0);
                    } else if (b == 68) { // left
                        move(-1, 0, 0);
                    }
                }
                break;
                
            case 122: // z
                move(0, 0, 3);
                break;
        
            case 99: // c
                if (swap(cposX, cposY, current.shape, current.rot) == 0) {
                    print();
                    exit(0);
                }
                break;
                    
            case 32:  //space
                gettimeofday(&startTime, NULL);

                if (harddrop() == 0) {
                    print();
                    exit(0);
                }
                break;
        }

        print();        
    }
}

int main() {
    srand(time(NULL));
    gettimeofday(&startTime, NULL);
    PrepareKbhit();
    printf("\x1b[2J");
    nextblock();
    render();
    print();
    input();
}