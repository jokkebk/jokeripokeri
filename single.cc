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

#define SH(v, s) ((s)>0 ? ((v)<<(s)) : ((v)>>(-(s))))

// Calculate a characteristic numbering hand cards 1-4 with no pairs or joker
// If h[0] is not joker:
// Bottom 2 bits: number of suited cards -- NOTE: NO FLUSHES!!!
// Next 13 bits: suits (1 means different suit)
// Next 13 bits: values (1 means card selected)
// Suit and value bits are masked to contain only cards relevant for straights
int enum_hand1(int *h) {
    if(h[0] != 52) {
        int mask = SH(0x1FF, h[0]/4-4);
        //cout << "Mask is " << bitset<28>(mask) << endl;
        if(h[0]/4<4) mask += 1<<12;
        else if(h[0]/4==12) mask += 0xF;
        mask &= (1<<13)-1;
        //cout << "Mask is " << bitset<28>(mask) << endl;
        mask = (mask << 15) + (mask << 2) + 3;
        //cout << "Mask is " << bitset<28>(mask) << endl;

        int r = 0;
        for(int i=1; i<5; i++) {
            //cout << "Card " << h[i]/4 << " ";
            r |= 1 << h[i]/4 + 15;
            if((h[i]&3) == (h[0]&3)) r++; // suited
            else r |= 1 << h[i]/4 + 2;
            //cout << "R is " << bitset<28>(r) << endl;
        }
        //cout << "R is " << bitset<28>(r) << endl;
        return r&mask;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    map<int,int> S;
    map<int,string> Shand;

    int h[5], cardV = 3; // 13 is joker
    //string vittu = "KH 2H 5H 6H 7H";
    //string_hand(h, vittu);
    //cout << hand_string(h) << " = " << enum_hand1(h) << " " << bitset<28>(enum_hand1(h)) << endl;
    //vittu = "KH 2D 3D 4D 5D";
    //string_hand(h, vittu);
    //cout << hand_string(h) << " = " << enum_hand1(h) << " " << bitset<28>(enum_hand1(h)) << endl;
    //return 0;

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
        int skip=0;

        for(int cards=15; cards<(1<<13); cards++) {
            if(__builtin_popcount(cards) != 4) continue; // 4 cards
            if(cards & (1<<cardV)) continue; // no selected card
            for(int suit=1; suit<16; suit++) { // skip flush
                //if(skip++ % 100 > 10) continue; // only calculate 1 %
                for(int i=0, c=1; i<13; i++) // construct hand
                    if(cards & (1<<i)) {
                        h[c] = i*4 + ((suit>>(c-1))&1);
                        c++; // thanks C
                    }

                if(win(h)) continue; // skip straights

                int en = enum_hand1(h);

                if(S.count(en)) continue; // done
                string handStr = hand_string(h); // TODO: remove

                //cout << bitset<28>(en) << " / " << en << " " << handStr << endl;

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
            }
        }

        cout << S.size() << " distincts hands found" << endl;
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
        //cout << count << endl;
        int cards = 0;
        for(int i=0; i<4; i++) {
            h[i+1] = hv[hi[i]];
            cards |= 1<<h[i+1]/4;
        }
        if(__builtin_popcount(cards) != 4) continue; // discrete cards
        if(cards & (1<<cardV)) continue; // no selected card
        if(win(h)) continue;
        count++;
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
            cout << bitset<28>(en) << " / " << en << endl;
            break;
        }

        if(++N % 100 == 0) cout << N << " ok..." << endl;
    } while(next_combi(hi, 4, hv.size()-1));
    cout << I << "/" << count << " iterations, tests: " << N << endl;
}
