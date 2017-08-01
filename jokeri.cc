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
        vector<pair<double,string>> ans;
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        cout << hand_num_norm(h) << endl;

        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            //n = __builtin_popcount(s)
            int I=0;
            double S=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            //cout << fixed << setprecision(8) << S/I << " " << hand_string(sel, n) << " " << I << endl;
            ans.push_back(make_pair(S/I, hand_string(sel, n)));
        }
        sort(ans.rbegin(), ans.rend());
        for(int i=0; i<5; i++) {
            double prob;
            string hand;
            tie(prob, hand) = ans[i]; //*max_element(ans.begin(), ans.end());
            cout << (i+1) << ". " << setw(8) << fixed << setprecision(5) << prob << " " << hand << endl;
        }
        //cout << win(h) << endl;
    }
}
