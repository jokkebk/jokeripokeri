#include <iostream>
#include <iomanip>
#include <vector>
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
    int h[5] = {0,1,2,3,4};
    int sample = 50000;
    if(argc > 1) sample = atoi(argv[1]);
    int I = myrand()%sample, N = 0; //2869685;
    if(argc > 2) I = atoi(argv[2]);
    double wins=0;

    precalc_init();

    cout << "Aiming for " << I << endl;
    clock_t start = clock();
    do {
        if(I++ % sample) continue;
        //print_hand(h);
        string handStr = hand_string(h);

        vector<int> left(53);
        vector<pair<double,string>> ans;
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

            if(n==1) {
                int h2[5], j=1;
                h2[0] = sel[0];
                for(int i=0, j=1; i<5; i++) if(h[i]!=h2[0]) h2[j++] = h[i];
                if(precalc_ok(h2)) {
                    int S = precalc_hand(h2), I = 194580;
                    //cout << "Reordered hand "; print_hand(h2);
                    //cout << fixed << setprecision(8) << (double)S/I << " " << S << endl;
                    //int en = enum_hand1(h2);
                    //cout << bitset<28>(en) << " / " << en << endl;
                    ans.push_back(make_pair((double)S/I, hand_string(h, 5, s)));
                    continue;
                }
            }

            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            //cout << fixed << setprecision(8) << S/I << " " << hand_string(sel, n) << " " << I << endl;
            ans.push_back(make_pair((double)S/I, hand_string(h, 5, s)));
        }
        double prob;
        string hand;
        tie(prob, hand) = *max_element(ans.begin(), ans.end());
        cout << handStr << " " << setw(8) << fixed << setprecision(5) << prob << " " << hand << endl;
        wins += prob;
        N += 1;
    } while(next_combi(h, 5, 52));
    double duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Calculated " << N << " hands in " << fixed << setprecision(5) << duration << "s" << endl;
    cout << "Average return " << wins/N << endl;
}
