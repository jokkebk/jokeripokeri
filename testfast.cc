#include <iostream>
#include <set>
#include <map>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <cstring>

#include "util.h"
#include "genhand.h"
#include "fastany.h"

using namespace std;

int main(int argc, char *argv[]) {
    clock_t start, end;
    double duration;

    FILE * out = NULL;
    if(argc > 1) {
        cout << "Write to " << argv[1] << endl;
        out = fopen(argv[1], "wt");
    }

    int num = 0;
    double sum = 0.0;

    start = clock();
    gen_normal_hands([&](int *h) { 
        int n = hand_num(h);
        int ansS;
        double ansP;

        ansS = optimal_selection_any(h, &ansP);
        sum += ansP;
        num += 1;

        if(out != NULL) fprintf(out, "%d %d %8.5f\n", n, ansS, ansP);
    });
    end = clock();
    duration = (double)(end-start)/CLOCKS_PER_SEC;
    cout << num << " in " << duration << "s: " << sum << endl;

    if(out) fclose(out);
}
