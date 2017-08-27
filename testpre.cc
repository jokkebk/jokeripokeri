#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <string>

#include "genhand.h"
#include "util.h"
#include "single.h"

using namespace std;

#define SH(v, s) ((s)>0 ? ((v)<<(s)) : ((v)>>(-(s))))

int main(int argc, char *argv[]) {
    int N=1000;
    srand(time(NULL));

    cout << "Running a simple test suite. Break any time with CTRL-C..." << endl;

    while(1) {
        for(int cardV = 0; cardV <= 13; cardV++) {
            int I=rand()%N, count=0;
            int h[5];

            h[0] = cardV==13 ? 52 : cardV*4;

            int hi[4] = {0, 1, 2, 3};
            vector<int> hv(52); // no joker in test
            iota(hv.begin(), hv.end(), 0);
            auto it = find(hv.begin(), hv.end(), h[0]);
            if(it != hv.end()) hv.erase(it);

            cout << "Testing " << hand_string(h, 1) << " with " << I << " offset." << endl;

            do { // values other than 'card'
                for(int i=0; i<4; i++) h[i+1] = hv[hi[i]];
                if(!precalc_ok(h)) continue;
                if(I++%N) continue; // Sample 1 % of relevant hands

                int score2 = precalc_hand(h);
                int en = enum_hand1(h);
                string hStr = hand_string(h);

                vector<int> left(53);
                iota(left.begin(), left.end(), 0);
                for(int i=0; i<5; i++) left.erase(find(left.begin(), left.end(), h[i]));

                int ci[4] = {0, 1, 2, 3};
                int score = 0, num = 0;
                do {
                    for(int i=0; i<4; i++) h[i+1] = left[ci[i]];
                    score += win(h);
                    num += 1;
                } while(next_combi(ci, 4, left.size()-1));

                if(score2 != score) {
                    cout << "CONFLICT: " << score << " vs. " << score2 << endl;
                    cout << "Hand: " << hStr << endl;
                    cout << bitset<28>(en) << " / " << en << endl;
                    break;
                }

                count++;
            } while(next_combi(hi, 4, hv.size()-1));
            cout << count << " ok..." << endl;
        }
    }
}
