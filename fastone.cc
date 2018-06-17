#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <tuple>
#include <map>
#include <algorithm>
#include <numeric>
#include <bitset>

#include "genhand.h"
#include "util.h"

using namespace std;

#define ONE_BIT(v) (!((v) & ((v)-1)))

int count_30s(int *h, int idx, bool debug = false) {
    int cmask = 0, omask = 0, val = h[idx]/4, suit = h[idx]&3, cnt = 0;

    for(int i=0, j=1; i<5; i++, j*=2) {
        if((h[i]&3) == suit)
            cmask |= 1<<(h[i]/4);
        else
            omask |= 1<<(h[i]/4);
    }

    if(debug) cout << "Analyzing " << val << " with " << bitset<13>(cmask) << endl;
    int start, step;
    if(val==12) { // ace
        start = -1;
        step = 9;
    } else {
        start = max(-1, val-4);
        step = 1;
    }
    for(; start<=min(8, val); start+=step) {
        if(debug) cout << "Start " << start << endl;
        for(int joker = 0; joker < 5; joker++) {
            if(joker && start+joker == val) {
                if(debug) cout << "  PAS " << start << " joker " << joker << endl;
                continue;
            }
            int smask = 0x1F - (joker ? (1<<joker) : 0);
            if(start < 0) smask = ((smask&1)<<12) + (smask/2&15);
            else smask <<= start;
            smask -= 1<<val;
            if(smask & cmask) {
                if(debug) cout << "  NOT " << start << " joker " << joker << " bits " << bitset<13>(smask) << endl;
            } else {
                if(debug) cout << "  YES " << start << " joker " << joker << " bits " << bitset<13>(smask) << endl;
                cnt++;
            }
        }
    }
    if(val>=9) {
        int needed = 0x1E00 - (1<<val);
        cout << "Checking extra joker: " << bitset<13>(cmask) << " vs " << bitset<13>(needed) << endl;
       if(!(cmask & needed)) cnt++; // add ?JQKA
    }

    return cnt;
}

int main() {
    int h[5];
    set<int> nums; // = gen_normal_hand_nums();
    map<int,int> ans;
    map<pair<int,int>, int> C;
    set<int> Is;
    time_t start, end;

    //string_hand(h, "2S 3S 4S 5S JD"); nums.insert(hand_num(h));
    //string_hand(h, "2S 3S 4S 5S QD"); nums.insert(hand_num(h));
    //string_hand(h, "2S 3S 4S 5S KD"); nums.insert(hand_num(h));
    //string_hand(h, "2S 3S 4S 5S AD"); nums.insert(hand_num(h));
    for(int i = 0; i < 10000; i++) nums.insert(rand()%1000000);
    cout << nums.size() << endl;

    start = clock();
    for(int num : nums) {
        num_hand(h, num);
        if(is_paired(h)) continue;
        if(h[4] == 52) continue;
        //cout << hand_string(h) << endl;

        vector<int> left(53);
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        for(int s=1, idx=0; s<32; s*=2, idx++) {
            map<int,int> tally;
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                int w = win(sel);
                if(w) tally[w]++;
                //if(w==30) cout << hand_string(sel) << endl;
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));

            //cout << "Adding " << tally[30] << " to ans[" << n << "] = " << ans[n] << endl;
            ans[num] += tally[30];
        }
    }
    end = clock();
    cout << (double)(end-start)/CLOCKS_PER_SEC << endl;

    start = clock();
    for(int num : nums) {
        num_hand(h, num);
        if(is_paired(h)) continue;
        if(h[4] == 52) continue;
        //cout << hand_string(h) << endl;

        int sum = 0;
        for(int s=1, idx=0; s<32; s*=2, idx++)
            sum += count_30s(h, idx);

        if(sum != ans[num]) 
            //cout << hand_string(h) << " (" << num << ") CORRECT " << ans[num] << " vs. " << sum << endl;
        //else
            cout << hand_string(h) << " (" << num << ") WRONG " << ans[num] << " vs. " << sum << endl;
    }
    end = clock();
    cout << (double)(end-start)/CLOCKS_PER_SEC << endl;

    return 0;

    cout << *(Is.begin()) << " and " << Is.size() << endl;
    //cout << N << " and " << C.size() << endl;

    for(auto p : C) {
        auto ii = p.first;
        cout << ii.first << " x" << ii.second << " total " << p.second << endl;
    }
}

            //double p;
            //int s;
            //tie(p, s) = ans[N];
