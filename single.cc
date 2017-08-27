#include <iostream>
#include <cstring>
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
// If h[0] is joker:
// Bottom 8 bits: representation of card 1-4 suits
// Next 13 bits: values
int enum_hand1(int *h) {
    int r = 0;

    if(h[0] == 52) { // Joker
        int no[4]={4,4,4,4}; // unity transform
        for(int i=1, ok=0; i<5; i++) {
            if(no[h[i]&3] > ok) no[h[i]&3] = ok++; // number suits as they come
            r = (r<<2) + no[h[i]&3];
        }
        for(int i=1; i<5; i++) r |= 1 << (h[i]/4 + 8);
    } else {
        int mask = SH(0x1FF, h[0]/4-4);
        if(h[0]/4<4) mask += 1<<12;
        else if(h[0]/4==12) mask += 0xF;
        mask &= (1<<13)-1;
        mask = (mask << 15) + (mask << 2) + 3;

        for(int i=1; i<5; i++) {
            r |= 1 << (h[i]/4 + 15);
            if((h[i]&3) == (h[0]&3)) r++; // suited
            else r |= 1 << (h[i]/4 + 2);
        }
        r &= mask;
    }

    return r;
}

bool precalc_ok(int *h) {
    int cards = 0;
    for(int i=0; i<5; i++) cards |= 1<<h[i]/4;
    if(__builtin_popcount(cards) != 5) return false; // discrete values
    if(h[4]==52) return false; // no jokers calculated
    return !win(h); // no straights or flushes eligible either
}

void precalc_card(map<int,int> & S, int cardV) {
    ostringstream ss;
    ss << "single" << cardV << ".dat";

    if(ifstream(ss.str())) {
        ifstream cache(ss.str());
        while(cache.good()) {
            int en, score;
            cache >> en >> score;
            S[en] = score;
        }
    } else {
        cout << "Rebuilding cache file " << ss.str() << endl;
        int h[5], hi[4] = {0, 1, 2, 3};
        vector<int> hv(52); // no joker
        iota(hv.begin(), hv.end(), 0);
        h[0] = (cardV==13) ? 52 : cardV*4;
        auto it = find(hv.begin(), hv.end(), h[0]);
        if(it != hv.end()) hv.erase(it);

        do {
            for(int i=0; i<4; i++) h[i+1] = hv[hi[i]];
            if(!precalc_ok(h)) continue;
            int en = enum_hand1(h);
            if(S.count(en)) continue; // done

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
        } while(next_combi(hi, 4, hv.size()-1));

        cout << S.size() << " distinct hands found" << endl;
        cout << "Writing cache file!" << endl;
        ofstream wCache(ss.str());
        for(auto p : S) wCache << p.first << " " << p.second << endl;
        wCache.close();
    }
}

vector<map<int,int>> calcC;

void precalc_init() {
    calcC.resize(14);

    for(int cardV=0; cardV<=13; cardV++)
        precalc_card(calcC[cardV], cardV);
}

int precalc_hand(int *h) {
    if(!calcC.size()) precalc_init();
    return calcC[h[0]/4][enum_hand1(h)];
}
