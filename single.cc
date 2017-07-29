#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
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

using namespace std;

// Calculate a characteristic numbering of a hand with no duplicates or joker
int enum_hand1(int *h) {
    int r = 0;
    for(int i=1; i<5; i++) {
        r |= 1 << h[i]/4;
        if((h[i]&3) != (h[0]&3)) r |= 1 << i-1 + 13;
    }
    return r;
}

int main(int argc, char *argv[]) {
    map<int,int> S;
    map<int,string> prevH;

    int h[5], cardV = 3; // 13 is joker

    if(argc>1) cardV = atoi(argv[1]);

    cout << "Calculating single precalculations for " << cardV << endl;

    h[0] = cardV==13 ? 52 : cardV*4;

    ostringstream ss;
    ss << "single" << cardV << ".dat";

    if(ifstream(ss.str())) {
        ifstream cache(ss.str());
        cout << "Found cache file " << ss.str() << endl;
        while(cache.good()) {
            int en, score;
            cache >> en >> score;
            S[en] = score;
        }
    } else {
        for(int cards=15; cards<(1<<13); cards++) {
            if(__builtin_popcount(cards) != 4) continue; // 4 cards
            if(cards & (1<<cardV)) continue; // no selected card
            for(int suit=1; suit<16; suit++) { // skip flush
                for(int i=0, c=1; i<13; i++) // construct hand
                    if(cards & (1<<i)) {
                        h[c] = i*4 + ((suit>>(c-1))&1);
                        c++; // thanks C
                    }

                if(win(h)) continue; // skip straights

                int en = enum_hand1(h);

                vector<int> left(53);
                iota(left.begin(), left.end(), 0);
                for(int i=0; i<5; i++) left.erase(find(left.begin(), left.end(), h[i]));

                int ci[4] = {0, 1, 2, 3};
                int score = 0;
                do {
                    for(int i=0; i<4; i++) h[i+1] = left[ci[i]];
                    score += win(h);
                } while(next_combi(ci, 4, left.size()-1));
                S[en] = score;
            }
        }

        cout << "Writing cache file!" << endl;
        ofstream wCache(ss.str());
        for(auto p : S) wCache << p.first << " " << p.second << endl;
        wCache.close();
    }

    cout << "Running a simple test suite..." << endl;

    srand(time(NULL));
    int I=rand()%100, N=0;

    int hi[4] = {0, 1, 2, 3};
    vector<int> hv(52); // no joker in test
    iota(hv.begin(), hv.end(), 0);
    hv.erase(find(hv.begin(), hv.end(), h[0]));

    int count=0;
    do { // values other than 'card'
        count++;
        for(int i=0; i<4; i++) h[i+1] = hv[hi[i]];
        int en = enum_hand1(h);
        if(!S.count(en)) continue; // Let's trust the enumeration this far

        if(I++%100) continue; // Sample 1 % of relevant hands
        string hStr = hand_string(h);

        vector<int> left(53);
        iota(left.begin(), left.end(), 0);
        for(int i=0; i<5; i++) left.erase(find(left.begin(), left.end(), h[i]));

        int ci[4] = {0, 1, 2, 3};
        int score = 0;
        do {
            for(int i=0; i<4; i++) h[i+1] = left[ci[i]];
            score += win(h);
        } while(next_combi(ci, 4, left.size()-1));

        if(S[en] != score) {
            cout << "CONFLICT: " << score << " vs. " << S[en] << endl;
            cout << "Hand: " << hStr << endl;
            cout << bitset<13>(en) << " / " << en << endl;
            break;
        }

        if(++N % 100 == 0) cout << N << " ok..." << endl;
    } while(next_combi(hi, 4, hv.size()-1));
    cout << I << "/" << count << " iterations, tests: " << N << endl;
}
