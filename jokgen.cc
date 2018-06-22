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

#define THREADED

#ifdef THREADED
#include <mingw.thread.h>
#endif

#include "util.h"
#include "fastone.h"

using namespace std;

void process(int *hnum, int n, int off, int step, double *ansP, int *ansS) {
    int h[5];
    for(int i=off; i<n; i+=step) {
        num_hand(h, hnum[i]);
        ansS[i] = optimal_selection(h, ansP+i);
    }
}

int main(int argc, char *argv[]) {
    set<int> nums = gen_normal_hand_nums();
    clock_t start;
    double duration;
    cout << nums.size() << endl;
    vector<int> hnum(nums.begin(), nums.end());

    //hnum.erase(hnum.begin()+10000, hnum.end());

    int *hnump = &hnum[0];
    double *ansP = new double[hnum.size()];
    int *ansS = new int[hnum.size()];

    start = clock();
#ifndef THREADED
    process(hnump, hnum.size(), 0, 1, ansP, ansS);
#else
    int threads = thread::hardware_concurrency(); // max out
    thread th[threads];
    cout << "Running " << threads << " threads" << endl;
    for(int i=0; i<threads; i++)
        th[i] = thread(process, hnump, hnum.size(), i, threads, ansP, ansS);
    for(int i=0; i<threads; i++) th[i].join(); // wait all threads to complete
#endif
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;

    cout << hnum.size() << " in " << fixed << setprecision(3) << duration << "s" << endl;

    float sumP = 0;
    int sumS = 0;
    long sumN = 0;

    FILE * out = NULL;
    if(argc > 1) {
        cout << "Write to " << argv[1] << endl;
        out = fopen(argv[1], "wt");
    }

    for(size_t i=0; i<hnum.size(); i++) {
        sumS += ansS[i];
        sumP += ansP[i];
        sumN += hnum[i];
        if(out != NULL)
            fprintf(out, "%d %d %8.5f\n", hnum[i], ansS[i], ansP[i]);
        //cout << hnum[i] << " " << ansS[i] << " " << setprecision(5) << ansP[i] << endl;
    }

    if(out != NULL) fclose(out);

    cout << sumP << " " << sumS << " " << sumN << endl;

    delete ansP;
    delete ansS;
}
