#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <tuple>
#include <algorithm>
#include <numeric>

#include "genhand.h"
#include "util.h"

using namespace std;

int main() {
    int h[5] = {0,1,2,3,4};

    while(input_hand(h)) {
        vector<int> left(53);
        vector<pair<double,int>> ans;
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        norm_hand(h);
        cout << hand_string(h) << " " << hand_num_norm(h) << endl;

        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            ans.push_back(make_pair((double)S/I, s));
        }
        sort(ans.rbegin(), ans.rend());
        for(int i=0; i<5; i++) {
            double p;
            int s;
            tie(p, s) = ans[i];
            cout << (i+1) << ". " << setw(8) << fixed << setprecision(5) << p << " " << hand_string(h, 5, s) << " " << s << endl;
        }
    }
}
