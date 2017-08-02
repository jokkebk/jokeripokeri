#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cstdlib>

#include "genhand.h"
#include "util.h"

using namespace std;

int main() {
    int h[5], h2[5], ok = 0;

    ifstream in("opti.txt");
    map<int,int> optiS;
    map<int,double> optiP;
    //map<int,int> cnt;

    while(in.good()) {
        int num, sel;
        double p;
        in >> num >> sel >> p;
        optiS[num] = sel;
        optiP[num] = p;
        int sc = __builtin_popcount(sel);
        //cnt[sc]++;
        //if(sc==5) {
        //    num_hand(h, num);
        //    cnt[win(h)]++;
        //}
    }

    //for(auto p : cnt) cout << p.first << " " << p.second << endl;

    iota(h, h+5, 0);
    double best=0, all=0, n=0;
    do {
        int onum = hand_num_norm(h);
        best += optiP[onum];
        all += win(h);
        //print_hand(h);
        n += 1;
    } while(next_combi(h, 5, 52));

    cout << n << " hands, " << best/n << " return, " << all/n << " straight up" << endl;
    return 0;
    srand(myrand());
    for(int i=0; i<5; i++) h[i] = rand();

    while(1) {
        int num = rand() % C(53,5), onum;
        num_hand(h, num);
        //print_hand(h);
        norm_hand(h);
        //print_hand(h);
        onum = hand_num(h);

        vector<int> left(53);
        pair<double,int> ans = make_pair(0,0);
        iota(left.begin(), left.end(), 0);
        for(int c : h) left.erase(find(left.begin(), left.end(), c));

        for(int s=1; s<32; s++) {
            int sel[5] = {52,52,52,52,52}, n=0, ci[4] = {0,1,2,3};
            for(int j=0; j<5; j++) if((s>>j)&1) sel[n++] = h[j];
            int S=0, I=0;
            do {
                for(int j=n; j<5; j++) sel[j] = left[ci[j-n]];
                S+=win(sel);
                I++;
            } while(next_combi(ci, 5-n, left.size()-1));
            ans = max(ans, make_pair((double)S/I, s));
        }

        if(optiS[onum] == ans.second) {
            if(++ok%100 == 0) cout << ok << " OK" << endl;
        } else if(optiS.count(onum) == 0) {
            cout << "OMG missing " << onum << endl;
        } else {
            cout << setw(8) << fixed << setprecision(5) << optiP[onum] << " " << hand_string(h, 5, optiS[onum]) << " <-- OPTIMAL" << endl;
            cout << setw(8) << fixed << setprecision(5) << ans.first << " " << hand_string(h, 5, ans.second) << endl;
        }
    }
}
