#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ctime>

#include "genhand.h"
#include "util.h"

using namespace std;

int main(int argc, char *argv[]) {
    int h[5] = {0,1,2,3,4};
    int sample = 10000;
    if(argc > 1) sample = atoi(argv[1]);
    srand(time(NULL));
    int I = rand()%sample;
    double wins=0, N=0;

    do {
        //if(I++ % sample) continue;
        vector<int> left(53);
        vector<pair<double,string>> ans;
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        clock_t start = clock();
        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            //n = __builtin_popcount(s)
            int I=0;
            double S=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            //cout << fixed << setprecision(8) << S/I << " " << hand_string(sel, n) << " " << I << endl;
            ans.push_back(make_pair(S/I, hand_string(h, 5, s)));
        }
        double duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
        cout << fixed << setprecision(8) << duration << "s" << endl;
        double prob;
        string hand;
        tie(prob, hand) = *max_element(ans.begin(), ans.end());
        cout << setw(8) << fixed << setprecision(5) << prob << " " << hand << endl;
        wins += prob;
        N += 1;
    } while(0 && next_combi(h, 5, 52));
    cout << "Average return " << wins/N << endl;
}
