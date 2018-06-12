#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <tuple>
#include <map>
#include <algorithm>
#include <numeric>

#include "genhand.h"
#include "util.h"

using namespace std;

int main() {
    int h[5];
    set<int> nums = gen_normal_hand_nums();
    map<pair<int,int>, int> C;
    set<int> Is;

    cout << nums.size() << endl;

    int N = 0;

    for(int n : nums) {
        num_hand(h, n);
        if(is_paired(h) || h[4] == 52) continue;
        //cout << hand_string(h) << endl;

        vector<int> left(53);
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        for(int s=1; s<32; s*=2) {
            map<int,int> tally;
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                int w = win(sel);
                if(w) tally[w]++;
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));

            Is.insert(I);
            for(auto p : tally)
                C[p]++;
        }
        if(++N > 100) break;
    }

    cout << *(Is.begin()) << " and " << Is.size() << endl;
    cout << N << " and " << C.size() << endl;

    for(auto p : C) {
        auto ii = p.first;
        cout << ii.first << " x" << ii.second << " total " << p.second << endl;
    }
    return 0;
}

            //double p;
            //int s;
            //tie(p, s) = ans[N];
