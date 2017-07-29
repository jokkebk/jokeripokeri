#include <iostream>
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
int enum_hand1(int *h) {
    int r = 0, s = 0;
    for(int i=1; i<5; i++) {
        r |= 1 << h[i]/4 + 4;
        if((h[i]&3) == (h[0]&3))
            r |= 1 << i-1;
    //    if(h[i] < h[0])
    //        r |= 1 << h[i]/4 + 4;
    //    else
    //        r |= 1 << h[i]/4 + 3;
    //    if((h[i]&3) == (h[0]&3)) r |= 1 << i-1;
    }
    return r;
}

// h[1] .. h[4] should be ordered in order for enum to work
//int enum_hand1(int *h) {
//    int vless = 0, vmore = 0, suit = 0;
//
//    for(int i=1; i<5; i++) {
//        int rel;
//
//        if(h[0]/4 == 12) { // Ace
//            if(h[i]/4 <= 3) // 2,3,4,5
//                vmore |= 1 << h[i];
//            else if(h[i] >= 8) // T,J,Q,K
//                vless |= 1 << 11-h[i];
//        } else { // Non-ace
//            if(h[i] < h[0])
//                vless |= 1 << (h[0]-h[i])/4-1;
//            else
//                vmore |= 1 << (h[i]-h[0])/4-1;
//            if(h[i]/4 == 12) // repeat ace on the bottom
//                vless |= 1 << h[0]/4;
//        }
//        if((h[i]&3) == (h[0]&3)) suit |= 1 << i-1;
//        //cout << bitset<8>(vless) << " " << bitset<8>(vmore) << " " << bitset<4>(suit) << endl;
//    }
//    //cout << bitset<8>(vless) << " " << bitset<8>(vmore) << " " << bitset<4>(suit) << endl;
//    return ((vless&15)<<8) + ((vmore&15)<<4) + suit;
//}

int main(int argc, char *argv[]) {
    int h[5], card = 3*4;
    int hv[4] = {0, 1, 2, 3};

    vector<int> hvals(13);
    iota(hvals.begin(), hvals.end(), 0);

    h[0] = card;
    if(card != 52) hvals.erase(find(hvals.begin(), hvals.end(), card/4));

    map<int,int> S;//ums
    map<int,string> prevH;

    int I=0, N=0;

    do { // values other than 'card'
        for(int i=0; i<4; i++) h[i+1] = hvals[hv[i]]*4;
        if(win(h)==30) continue; // skip straights

        for(int s=1; s<0xFF; s++) { // suits except flush (all 0)
            for(int i=0; i<4; i++) h[i+1] = hvals[hv[i]]*4 + ((s>>i*2)&3);
            int en = enum_hand1(h);
            string hStr = hand_string(h);
            //if(I++%10) continue;
            //if(en != 11782) continue;
            if(S.count(en)) continue; // already calculated
            //S[en]++;
            //continue;

            vector<int> left(53);
            iota(left.begin(), left.end(), 0);
            for(int i=0; i<5; i++) left.erase(find(left.begin(), left.end(), h[i]));

            //map<int,int> C;
            int ci[4] = {0, 1, 2, 3};
            int score = 0;
            do {
                for(int i=0; i<4; i++) h[i+1] = left[ci[i]];
                score += win(h);
            } while(next_combi(ci, 4, left.size()-1));

            //if(S.count(en) == 0) {
                S[en] = score;
                prevH[en] = hStr;
            //S[en] = score;
            //for(auto p : C) {
            //    cout << p.first << "*" << p.second << endl;
            //    S[p.first].insert(p.second);
            //}
        }
    } while(next_combi(hv, 4, hvals.size()-1)); // no joker

    return 0;

    I=0, N=0;

    for(int i=0; i<4; i++) hv[i] = i;

    do { // values other than 'card'
        for(int i=0; i<4; i++) h[i+1] = hvals[hv[i]]*4;
        if(win(h)==30) continue; // skip straights

        for(int s=1; s<0xFF; s++) { // suits except flush (all 0)
            for(int i=0; i<4; i++) h[i+1] = hvals[hv[i]]*4 + ((s>>i*2)&3);
            string hStr = hand_string(h);
            int en = enum_hand1(h);
            if(I++%1000) continue;

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
                cout << "Hands: " << hStr << " vs. " << prevH[en] << endl;
                cout << bitset<13>(en) << " / " << en << endl;
                break;
            }

            if(++N % 100 == 0) cout << N << " ok..." << endl;
        }
    } while(next_combi(hv, 4, hvals.size()-1)); // no joker
    //cout << S.size() << endl;

    //for(auto p : S) cout << p.first << " " << p.second << endl;
}
