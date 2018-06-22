#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <tuple>
#include <map>
#include <algorithm>
#include <numeric>
#include <bitset>

#include "util.h"

#include "fastone.h"

using namespace std;

int count_30s(int *h, int idx, bool debug = false) {
    int cmask = 0, omask = 0, val = h[idx]/4, suit = h[idx]&3, cnt = 0;

    for(int i=0, j=1; i<5; i++, j*=2) {
        if((h[i]&3) == suit)
            cmask |= 1<<(h[i]/4);
        else
            omask |= 1<<(h[i]/4);
    }

    //if(debug) cout << "Analyzing " << val << " with " << bitset<13>(cmask) << endl;
    int start, step;
    if(val==12) { // ace
        start = -1;
        step = 9;
    } else {
        start = max(-1, val-4);
        step = 1;
    }
    for(; start<=min(8, val); start+=step) {
        //if(debug) cout << "Start " << start << endl;
        for(int joker = 0; joker < 5; joker++) {
            if(joker && start+joker == val) {
                //if(debug) cout << "  PAS " << start << " joker " << joker << endl;
                continue;
            }
            int smask = 0x1F - (joker ? (1<<joker) : 0);
            if(start < 0) smask = ((smask&1)<<12) + (smask/2&15);
            else smask <<= start;
            smask -= 1<<val;
            if(smask & cmask) {
                //if(debug) cout << "  NOT " << start << " joker " << joker << " bits " << bitset<13>(smask) << endl;
            } else {
                //if(debug) cout << "  YES " << start << " joker " << joker << " bits " << bitset<13>(smask) << endl;
                cnt++;
            }
        }
    }
    if(val>=9) {
        int needed = 0x1E00 - (1<<val);
        //if(debug) cout << "Checking extra joker: " << bitset<13>(cmask) << " vs " << bitset<13>(needed) << endl;
       if(!(cmask & needed)) cnt++; // add ?JQKA
    }

    return cnt;
}

int count_3s(int *h, int idx, bool debug = false) {
    int mask = 0, val = h[idx]/4, cnt = 0;

    for(int i=0, j=1; i<5; i++, j*=2)
        mask |= 1<<h[i]/4;

    mask += (mask&15) << 13; // replicate 2345 above ace
    int c[9];
    for(int i=-4; i<5; i++) {
        if(val + i < -1) {
            c[i+4] = 0; // out of range
        } else if(val + i > 12 && val != 12) {
            c[i+4] = 0; // only reach over 12 if ace selected
        } else if(val + i == -1) {
            c[i+4] = (mask & 0x1000) ? 3 : 4;
        } else if(!i) {
            c[i+4] = 1;
        } else {
            c[i+4] = ((mask >> (val+i)) & 1) ? 3 : 4;
        }
    }

    int step = (val==12) ? 4 : 1; // only A2345 and TJQKA with ace

    for(int i=0; i<5; i+=step) {
        int straights = c[i] * c[i+1] * c[i+2] * c[i+3] * c[i+4];
        cnt += straights;
        // Extra joker from 0 when it is T and val is J+
        for(int joker = (val >=9 && val-4+i == 8) ? 0 : 1; joker < 5; joker++) {
            if(i+joker == 4 || !c[i+joker]) continue; // cannot replace selected card
            cnt += straights / c[i+joker];
        }
    }

    //if(debug) cout << bitset<17>(mask) << endl;
    //if(debug) for(int v : c) cout << v << " ";
    //if(debug) cout << endl;

    return cnt;
}

int count_4s(int *h, int idx, bool debug = false) {
    int suited = 14; // 13 suited cards + joker

    for(int i=0; i<5; i++)
        if((h[i] & 3) == (h[idx] & 3))
            suited--;

    return C(suited, 4);
}

#define FASTONE

// Find optimal selection for a *normalized* hand
int optimal_selection(int *h, double *p) {
    int left[48];
	int ansS = 0;
	double ansP = 0.0;

	int c, lp, hp;
    for(c=0, lp=0, hp=0; c<53; c++)
        if(hp < 5 && h[hp] == c) hp++; else left[lp++] = c;

    bool paired = is_paired(h);

    for(int s=1; s<32; s++) {
		// no single selection optimums with a pair or better in hand
		if(ONE_BIT(s) && paired) continue; 
        // With joker, you have to select the joker, not toss it
        if(ONE_BIT(s) && h[4]==52 && s<16) continue;

        int S=0, I=0;

#ifdef FASTONE
        if(ONE_BIT(s) && !paired && h[4] != 52) { // use fast counting
            int idx=0;
            while(s>>idx) idx++;
            idx--;
            int c50s = 1, c15s = 220, c8s = 468, c2s = 18046;
            int c30s = count_30s(h, idx); // straight flushes
            int c3s = count_3s(h, idx) - c30s; // straights, don't double count
            int c4s = count_4s(h, idx) - c30s; // flushes, don't double count

            S = 50 * c50s + 30 * c30s + 15 * c15s + 8 * c8s + 4 * c4s + 3 * c3s + 2 * c2s;
            I = C(48, 4);
        } else {
#endif
            int sel[5], ci[4] = {0,1,2,3}, n=0;
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, 53-5-1));
#ifdef FASTONE
        }
#endif

        double p = (double)S/I;

        if(p > ansP) {
            ansP = p;
            ansS = s;
        }
    }

	if (p) *p = ansP;
	return ansS;
}
