#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <utility>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <cstring>
#include <mingw.thread.h>

#include "genhand.h"
#include "util.h"
#include "single.h"

using namespace std;

void optimal_selection(int *h, int *sel, double *prob) {
    *prob = 0;

    int left[48];
    for(int c=0, lp=0, hp=0; c<53; c++)
        if(h[hp] == c) hp++; else left[lp++] = c;

    for(int s=1; s<32; s++) {
        int sel[5], n=0;
        for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

        int S=0, ci[4] = {0,1,2,3};
        do {
            for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
            S+=win(sel);
        } while(next_combi(ci, 5-n, 53-5-1));

        double p = (double)S/C(53-5, 5-n);
        if(p > *prob) {
            *prob = p;
            *sel = s;
        }
    }
}

void process(int *hnum, int n, int off, int step, double *ansP, int *ansS) {
    int h[5];

    for(int i=off; i<n; i+=step) {
        num_hand(h, hnum[i]);
        optimal_selection(h, ansS+i, ansP+i);
    }
}

int main(int argc, char *argv[]) {
    clock_t start;
    double duration;
    int sample=1000;
    int I=0;//myrand()%sample;
    unsigned maxthreads = 999;

    if(argc > 1) sample = atoi(argv[1]);
    if(argc > 2) maxthreads = atoi(argv[2]);

    cout << "Sampling every " << sample << "th starting from " << I << endl;

    int h[5] = {0,1,2,3,4};
    set<int> seen;
    start = clock();
    do {
        int num = hand_num_norm(h);
        seen.insert(num);
    } while(next_combi(h, 5, 52));
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << seen.size() << " hands in " << fixed << setprecision(3) << duration << "s" << endl;


    vector<int> hnum(seen.begin(), seen.end());
    int *hnump = &hnum[0];
    double *ansP = new double[hnum.size()];
    int *ansS = new int[hnum.size()];

    start = clock();
    //process(hnump, hnum.size(), I, sample, ansP, ansS);
    int threads = min(maxthreads, thread::hardware_concurrency());
    thread th[threads];
    cout << threads << " threads" << endl;
    for(int i=0; i<threads; i++)
        th[i] = thread(process, hnump, hnum.size(), i*sample, sample*threads, ansP, ansS);
    for(int i=0; i<threads; i++) th[i].join(); // wait all threads to complete
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    //for(int i=0; i<(int)hnum.size(); i++)
    //    if(ansS[i]) cout << hnum[i] << " " << ansS[i] << " " << setprecision(8) <<
    //        ansP[i] << endl;
    cout << fixed << setprecision(3) << duration << "s" << endl;

    float sum = 0;

    for(int i=0; i<hnum.size(); i++) sum += ansP[i];

    cout << sum << endl;

    delete ansP;
    delete ansS;
}
