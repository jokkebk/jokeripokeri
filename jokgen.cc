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

#include "genhand.h"
#include "util.h"
#include "single.h"

using namespace std;

int main(int argc, char *argv[]) {
    int h[5] = {0,1,2,3,4}, h2[5];
    int sample=10000;
    int I=myrand()%sample, N=0;

    precalc_init();

    set<int> seen;
    clock_t start = clock();
    do {
        int num = hand_num_norm(h);

        if(seen.count(num)) continue;
        seen.insert(num);

        vector<int> left(53);
        vector<pair<double,string>> ans;
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        pair<double,int> best = make_pair(0.0, 0);

        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

            if(n==1) {
                h2[0] = sel[0];
                for(int i=0, j=1; i<5; i++) if(h[i]!=h2[0]) h2[j++] = h[i];
                if(precalc_ok(h2)) {
                    int S = precalc_hand(h2), I = 194580;
                    best = max(best, make_pair((double)S/I, s));
                    continue;
                }
            }

            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            best = max(best, make_pair((double)S/I, s));
        }
        double prob;
        int sel;
        tie(prob, sel) = best;
        cout << num << " " << sel << " " << setprecision(8) << prob << endl;
    } while(next_combi(h, 5, 52));
    double duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Calculated " << N << " hands in " << fixed << setprecision(3) << duration << "s" << endl;
}
