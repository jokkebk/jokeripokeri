#include <iostream>
#include <functional>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <cstring>

#include "util.h"
#include "fastone.h"

using namespace std;

inline int countOnes5b(int s) { // it's fast!
    s = (s&(1+4+16)) + ((s&(2+8))>>1);
    return (s>>4) + ((s>>2)&3) + (s&3);
}

// Find optimal selection for a *normalized* hand
int optimal_selection_test(int *h, double *p, int ones) {
    int left[48];
	int ansS = 0;
	double ansP = 0.0;

	int c, lp, hp;
    for(c=0, lp=0, hp=0; c<53; c++)
        if(hp < 5 && h[hp] == c) hp++; else left[lp++] = c;

    bool paired = is_paired(h);

    for(int s=1; s<32; s++) {
        if(countOnes5b(s) != ones) continue;
		if(ONE_BIT(s) && paired) continue; 
        if(h[4]==52 && s<16) continue;

        int S=0, I=0;

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
            int sel[5], ci[4] = {0,1,2,3}, n=0;
            bool suited = true;
            for(int j=0; j<5; j++) if((s>>j)&1) {
                sel[n++] = h[j];
                if((sel[n-1]&3) != (sel[0]&3)) suited = false;
            }

            if(suited) {
                do {
                    for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                    S+=win(sel);
                    I++;
                } while(next_combi(ci, 5-n, 53-5-1));
            } else {
                do {
                    for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                    S+=win_mixed(sel);
                    I++;
                } while(next_combi(ci, 5-n, 53-5-1));
            }
        }

        double p = (double)S/I;

        if(p > ansP) {
            ansP = p;
            ansS = s;
        }
    }

	if (p) *p = ansP;
	return ansS;
}

map<int,int> count_wins(int *h, int s) {
    map<int,int> cnt;

    int left[48];

	int c, lp, hp;
    for(c=0, lp=0, hp=0; c<53; c++)
        if(hp < 5 && h[hp] == c) hp++; else left[lp++] = c;

    int sel[5], ci[4] = {0,1,2,3}, n=0;

    for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

    do {
        for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
        cnt[win(sel)]++;
    } while(next_combi(ci, 5-n, 53-5-1));

    return cnt;
}

// Win with paired cards, no flush or straight
// 0=nothing
// 1=two pair or set
// 2=full house
// 3=quads
// 4=five of a kind
int win_pairs(int *a) {
    int h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);

	int c[5] = { 1,0,0,0,0 }, n = 0;

    for (int i = 1; i < 5; i++) {
        if((h[i]>>2) != (h[i-1]>>2)) n++;
        c[n]++;
    }


    // possible counts: 41 14 32 23 311 131 113 221 212 122 2111 1211 1121 1112 11111

    if(n==4) return 0;
    if(n==3) return h[4]==52 ? 1 : 0;
    if(n==2) { // 311 131 113 221 212 122
        if(h[4]==52) // joker, i.e. 311 131 or 221
            return c[0]==2 ? 2 : 3;
        else
            return 1;
    }

    if(c[0]==4 || c[1]==4) return h[4]==52 ? 4 : 3;

    return 2;
}

// Calculate characteristic mask for a hand. For two hands and
// selections with same mask, they should yield same amount of pairs etc.
// Hand needs to be sorted.
int pair_mask(int *h, int s, bool debug=false) {
	int numsel[5] = { 0,0,0,0,0 }, n = 0;
    int jokersel = (h[4]==52) ? ((s&16) ? 3 : 1) : 0;

    numsel[0] = 8 + (s&1); // hi3:low3 = count(3):selected(3)

    for (int i = 1; i < 5; i++) {
        if((h[i]>>2) != (h[i-1]>>2)) n++;
        numsel[n] += 8 + ((s>>i)&1);
    }

    // possible counts: 41 14 32 23 311 131 113 221 212 122 2111 1211 1121 1112 11111

    // if n==4 counts are 11111
    // n==3 is 2111 1211 1121 1112
    if(n==3 && numsel[2] < numsel[3]) swap(numsel[2], numsel[3]);
    if(n>1 && numsel[1] < numsel[2]) swap(numsel[1], numsel[2]);
    if(numsel[0] < numsel[1]) swap(numsel[0], numsel[1]);
    // rinse and repeat to get 122 into 221
    if(n>1 && numsel[1] < numsel[2]) swap(numsel[1], numsel[2]);

    if(debug) {
        cout << hand_string(h, 5, s) << " has " << n+1 << " different values:" << endl;
        for(int i=0; i<=n; i++) cout << bitset<6>(numsel[i]) << endl;
    }

    if((numsel[1]>>3)>1)
        return jokersel + // 2 bits for joker (1) and selected (2)
            ((numsel[0])<<2) + // 6 bits for mode card and select count
            ((numsel[1])<<8) + // 6 bits for mode card and select count
            (((numsel[2]&7)+(numsel[3]&7)+(numsel[4]&7))<<14); // selected 1s
    else
        return jokersel + // 2 bits for joker (1) and selected (2)
            ((numsel[0])<<2) + // 6 bits for mode card and select count
            ((8+(numsel[1]&7)+(numsel[2]&7)+(numsel[3]&7)+(numsel[4]&7))<<8) + 0xFF0000;
}

map<int,long long> cache_pair;
//map<int,int> cache_num;
//map<int,int> cache_sel;

// Count different types of pairs etc. resulting from a selection
// int cnt[5] = {none, two pair+set, full house, quads, five of a kind}
void count_pair_wins(int *h, int s, int *cnt) {
    int pm = pair_mask(h, s);

    if(cache_pair.count(pm)) {
        long long p = cache_pair[pm];
        cnt[1] = (p >> 0) & 0xFFFF;
        cnt[2] = (p >> 16) & 0xFFFF;
        cnt[3] = (p >> 32) & 0xFFFF;
        cnt[4] = (p >> 48);
        return;
        //int cnt2[5];
        /*for(int i=1; i<5; i++) {
            if(cnt[i] != cnt2[i]) {
                cout << "Cache miss " << hand_string(h, 5, s) << ": cnt[" << i << "] is " << cnt[i] << " and " << cnt2[i] << endl;
                cout << hand_num(h) << endl;
                int h2[5];
                num_hand(h2, cache_num[pm]);
                cout << "Previous hand " << hand_string(h2, 5, cache_sel[pm]) << endl;
                cout << bitset<16>(pair_mask(h, s, true)) << " vs " << bitset<16>(pair_mask(h2, cache_sel[pm], true)) << endl;
                map<int,int> w = count_wins(h, s);
                map<int,int> w2 = count_wins(h2, cache_sel[pm]);
                for(auto p : w) {
                    //if(w2.count(p.first) && p.second != w2[p.first]) {
                        cout << p.first << ": " << p.second << " vs. " << w2[p.first] << endl;
                        //exit(1);
                    //}
                }
                exit(1);
            }
        }
        */
    }

    int left[48];

	int c, lp, hp;
    for(c=0, lp=0, hp=0; c<53; c++)
        if(hp < 5 && h[hp] == c) hp++; else left[lp++] = c;

    int sel[5], ci[4] = {0,1,2,3}, n=0;

    for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];

    do {
        for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
        int w2 = win_pairs(sel);
        cnt[w2]++;
        //int w = win(sel);
        //if(w==2) w=1;
        //else if(w==8) w=2;
        //else if(w==15) w=3;
        //else if(w==50) w=4;
        //else w=0;
        //if(w != w2) {
        //    cout << "FAIL with " << hand_string(sel) << ": " << w << " vs. " << w2 << endl;
        //    exit(1);
        //}
    } while(next_combi(ci, 5-n, 53-5-1));

    cache_pair[pm] =
        ((long long)cnt[1] & 0xFFFF) +
        ((long long)cnt[2] << 16) +
        ((long long)cnt[3] << 32) +
        ((long long)cnt[4] << 48);
}

// Count possible flushes -- includes straights
// Relies on a sorted hand.
int count_4sel(int *h, int sel) {
    int suit=-1;

    if(h[4]==52 && sel==16) { // only joker selected
        int s[4] = {13, 13, 13, 13}, cnt=0;
        for(int i=0; i<4; i++) s[h[i]&3]--;
        for(int i=0; i<4; i++) {
            //cout << "C(" << s[i] << ", 4) = " << C(s[i], 4) << endl;
            cnt += C(s[i], 4);
        }
        return cnt;
    }

    for(int i=0, s=sel; i<5; i++, s>>=1)
        if(s&1) { suit = h[i]&3; break; }

    int suited = 14, left = 5;

    for(int i=0; i<5; i++) {
        if((h[i]&3) == suit || h[i]==52) {
            if(sel & (1<<i)) left--;
            suited--;
        } else if(sel & (1<<i)) return 0;
    }
    //cout << suited << " suited " << suit << endl;

    return C(suited, left);
}

// Count possible straight flushes
int count_30sel(int *h, int sel) {
    int suit=-1;

    int from, to;

    if(h[4]==52 && sel==16) { // only joker
        from=0; to=3;
    } else {
        for(int i=0, s=sel; i<5; i++, s>>=1)
            if(s&1) { suit = h[i]&3; break; }
        from=to=suit;
    }

    int cnt = 0;

    for(suit=from; suit<=to; suit++) {
        int has=0, hasnt=0;

        for(int i=0; i<5; i++) {
            int v = 1 << (h[i]>>2);
            if((h[i]&3) == suit || v==0x2000) {
                if(sel & (1<<i))
                    has |= v;
                else
                    hasnt |= v;
            } else if(sel & (1<<i)) return 0;
        }

        for(int joker = 0; joker < 5; joker++) {
            int m0 = joker ? 0x2000 : 0;
            int m1 = 31 - (joker ? (1<<joker) : 0);
            for(int i=0; i<10; i++) {
                int m = m0 + (i ? (m1<<(i-1)) : (0x1000 + (m1>>1)));
                if((has & m)==has && !(hasnt & m)) cnt++;
            }
        }

        if((has & 0x3E00)==has && !(hasnt & 0x3E00)) cnt++; // JQKA?
    }

    return cnt;
}

// Count possible straights -- includes straight flushes
int count_3sel(int *h, int sel) {
    int indeck[14] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1};
    int has=0;

    for(int i=0; i<5; i++) {
        int v = h[i]>>2;
        int m = 1 << v;
        if(sel & (1<<i)) {
            if(has & m) return 0; // paired selection
            has |= m;
        } else {
            indeck[v]--;
        }
    }

    int cnt = 0;

    for(int joker = 0; joker < 5; joker++) {
        int m0 = joker ? 0x2000 : 0;
        int m1 = 31 - (joker ? (1<<joker) : 0);
        int max = joker==4 ? 11 : 10; // include JQKA?
        for(int i=0; i<max; i++) {
            int m = m0 + (i ? (m1<<(i-1)) : (0x1000 + (m1>>1)));
            if((has & m)!=has) continue;

            int left = m - has, p = 1;

            // Wasteful, but who cares?
            for(int v=0; left; v++, left>>=1) if(left & 1) p *= indeck[v];

            cnt += p;
        }
    }

    return cnt;
}

// Generate base cases for different hands yielding different amount of
// paired hands (five, four, three of a kind, full house, two pairs)
// Suits don't matter, nor do values, only relative numbers of values.
// Still, we return suits to support hand enumeration
void gen_base_pairs(function<void(int *)> f, bool joker) {
    int h[5];

    for(int top=1; top<=4; top++) {
        for(int i=0; i<top; i++) h[i] = i;
        for(int sec=1; sec <= top && top+sec <= 5; sec++) {
            for(int i=top; i<top+sec; i++) h[i] = 4 + (i-top);
            for(int i=top+sec; i<5; i++) h[i] = 8 + (i-top-sec)*4;
            f(h);
            if(joker && !(top==3 && sec==2)) { // 22233 and 22234
                h[4]=52;
                f(h);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    clock_t start, end;
    double duration;
    int h[5];
    vector<int> bp;
    map<int,int> cnt;

    //start = clock();
    //gen_base_pairs([&](int *h) { bp.push_back(hand_num(h)); }, true);
    //for(int n : bp) {
    //    num_hand(h, n);
    //    cout << hand_string(h) << endl;
    //    for(int s = 1; s<2; s++) {
    //        int cnt1[5] = {0, 0, 0, 0, 0};
    //        map<int,int> w = count_wins(h, s);
    //        int cnt2[5] = {0, w[2], w[8], w[15], w[50]};

    //        count_pair_wins(h, s, cnt1);

    //        for(int i=0; i<5; i++) {
    //            cout << "  " << cnt1[i] << " vs " << cnt2[i] << endl;
    //        }
    //        //int pm = pair_mask(h, s);
    //        //cnt[pm]++;
    //    }
    //}
    //end = clock();
    //duration = (double)(end-start)/CLOCKS_PER_SEC;
    //cout << cnt.size() << " in " << duration << "s" << endl;

    set<int> hset = gen_normal_hand_nums();
    vector<int> hnum(hset.begin(), hset.end());
    //vector<int> hnum(10000);
    //srand(time(NULL));
    //for(int & n : hnum) {
    //    num_hand(h, (rand() * 65536 + rand()) % C(53,5));
    //    norm_hand(h);
    //    n = hand_num(h);
    //}

    long long sum = 0;

        /*for(auto p : w) 
            if(w2.count(p.first) && p.second != w2[p.first]) {
                cout << "Testing " << hand_string(h, 5, s) << " (" << n << "): " << endl;
                cout << p.first << ": " << p.second << " vs. " << w2[p.first] << endl;
                exit(1);
            }*/

    start = clock();
    for(int n : hnum) {
        num_hand(h, n);

        for(int s=1; s<32; s++) {
            int cnt[5] = {0, 0, 0, 0, 0};
            count_pair_wins(h, s, cnt);
            sum += cnt[1] * 2 + cnt[2] * 8 + cnt[3] * 15 + cnt[4] * 50;
            int c30s = count_30sel(h, s);
            sum += 30 * c30s;
            sum += 4 * (count_4sel(h, s) - c30s);
            sum += 3 * (count_3sel(h, s) - c30s);
        }
    }
    end = clock();
    duration = (double)(end-start)/CLOCKS_PER_SEC;
    cout << hnum.size() << " in " << duration << "s: " << sum << endl;

    return 0;
    sum = 0;
    start = clock();
    for(int n : hnum) {
        for(int s=1; s<32; s++) {
            num_hand(h, n);
            map<int,int> w = count_wins(h, s);
            for(auto p : w) sum += p.first * p.second;
        }
    }
    end = clock();
    duration = (double)(end-start)/CLOCKS_PER_SEC;
    cout << hnum.size() << " in " << duration << "s: " << sum << endl;

    cout << cache_pair.size() << " unique pair thingies" << endl;
    /*
    for(int ones = 1; ones <= 5; ones++) {
        start = clock();
        for(auto n : hnum) {
            num_hand(h, n);
            optimal_selection_test(h, NULL, ones);
        }
        end = clock();
        duration = (double)(end-start)/CLOCKS_PER_SEC;
        cout << ones << " ones took " << duration << " seconds" << endl;
    }
    */
}
