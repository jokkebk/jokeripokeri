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
// Bottom 2 bits: number of suited cards
// Next 13 bits: suits (1 means different suit)
// Next 13 bits: cards (1 means card selected)
int enum_hand1(int *h) {
    int r = 0;
    for(int i=1; i<5; i++) {
        r |= 1 << h[i]/4 + 15;
        if((h[i]&3) == (h[0]&3)) r++; // suited
        else r |= 1 << h[i]/4 + 2;
    }
    return r;
}

int main(int argc, char *argv[]) {
    map<int,int> S;
    map<int,string> Shand;


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
        // We can speed up calculation of other cards than joker
        // by just throwing away "outside straight range" card
        // value information (needs suitable enumeration function)
        string cheatStr[13] = {
            "1000000011110",
            "1000000111101",
            "1000001111011",
            "1000011110111",
            "0000111101111",
            "0001111011110",
            "0011110111100",
            "0111101111000",
            "1111011110000",
            "1110111100000",
            "1101111000000",
            "1011110000000",
            "0111100001111"
        };
        int cheatMask = 0;
        if(cardV != 13) {
            cheatMask = (int)(bitset<13>(cheatStr[cardV]).to_ulong());
            cheatMask = (cheatMask << 2) + (cheatMask << 15) + 3;
            cout << "Using cheat mask " << bitset<30>(cheatMask) << endl;
        }
        map<int,int> cheatS;
        map<int,string> cheatLast;

        int skip=0;

        for(int cards=15; cards<(1<<13); cards++) {
            if(__builtin_popcount(cards) != 4) continue; // 4 cards
            if(cards & (1<<cardV)) continue; // no selected card
            for(int suit=1; suit<16; suit++) { // skip flush
                if(skip++ % 100) continue; // only calculate 1 %
                for(int i=0, c=1; i<13; i++) // construct hand
                    if(cards & (1<<i)) {
                        h[c] = i*4 + ((suit>>(c-1))&1);
                        c++; // thanks C
                    }

                if(win(h)) continue; // skip straights

                int en = enum_hand1(h);

                string handStr = hand_string(h); // TODO: remove
                int cheatEn = cheatMask & en; // 0 if card is joker
                //if(cheatMask && cheatS.count(cheatEn)) {
                //    S[en] = cheatS[cheatEn];
                //    continue;
                //}

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
                Shand[en] = handStr;

                if(cheatEn) {
                    if(cheatS.count(cheatEn) && cheatS[cheatEn] != score) {
                        cout << "CONFLICT! " << score << " vs. cheatS " << cheatS[cheatEn] << endl;
                        cout << "enums " << bitset<28>(en) << " vs. cheatEn " << bitset<28>(cheatEn) << endl;
                        cout << "had cards " << cheatLast[cheatEn] << " vs. now " << handStr << endl;
                        return 0;
                    }
                    cheatS[cheatEn] = score; // store in cheat sheet
                    cheatLast[cheatEn] = handStr; // store in cheat sheet
                }
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
    auto it = find(hv.begin(), hv.end(), h[0]);
    if(it != hv.end()) hv.erase(it);

    int count=0;
    do { // values other than 'card'
        count++;
        //cout << count << endl;
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
            cout << "Hand: " << hStr << " vs. " << Shand[en] << endl;
            cout << bitset<13>(en) << " / " << en << endl;
            break;
        }

        if(++N % 100 == 0) cout << N << " ok..." << endl;
    } while(next_combi(hi, 4, hv.size()-1));
    cout << I << "/" << count << " iterations, tests: " << N << endl;
}
