#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <ctime>
#include <cstring>

#include "util.h"

int main() {
    int h[5] = {0,1,2,3,4};
    clock_t start, end;
    long long wins;
    int I;

    /*
    wins = 0; I = 0;
    start = clock();
    for(int i=0; i<10; i++) {
        for(int i=0; i<5; i++) h[i] = i;
        do {
            wins += win2(h);
            I++;
        } while(next_combi(h, 5, 53-1));
    }
    end = clock();
    printf("%d hands in %f seconds, win sum %I64d\n", I, (double)(end - start)/CLOCKS_PER_SEC, wins);
    */

    wins = 0; I = 0;
    start = clock();
    for(int i=0; i<10; i++) {
        for(int i=0; i<5; i++) h[i] = i;
        do {
            wins += win(h);
            I++;
        } while(next_combi(h, 5, 53-1));
    }
    end = clock();
    printf("%d hands in %f seconds, win sum %I64d (should be 720462 per round)\n", I, (double)(end - start)/CLOCKS_PER_SEC, wins);
}
