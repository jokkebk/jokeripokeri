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

#include "genhand.h"
#include "util.h"
#include "single.h"

using namespace std;

int main(int argc, char *argv[]) {
    int h[5] = {0,1,2,3,4};
    int sample=1000;
    int I=myrand()%sample, N=0;

    if(argc > 1) sample = atoi(argv[1]);

    set<int> seen;
    clock_t start = clock();
    do {
        int num = hand_num_norm(h);

        if(seen.count(num)) continue;
        seen.insert(num);
        if(I++ % sample) continue;

        pair<double,int> best = make_pair(0.0, 0);

        vector<int> left(53);
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        for(int s=1; s<32; s++) {
            int sel[5], n=0;
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

            int S=0, ci[4] = {0,1,2,3};
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
            } while(next_combi(ci, 5-n, 53-5-1));
            best = max(best, make_pair((double)S/C(53-5, 5-n), s));
        }
        double prob;
        int sel;
        tie(prob, sel) = best;
        cout << num << " " << sel << " " << setprecision(8) << prob << endl;
        N++;
    } while(next_combi(h, 5, 52));
    double duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << N << " hands in " << fixed << setprecision(3) << duration << "s" << endl;
}
