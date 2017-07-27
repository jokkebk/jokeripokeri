#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <ctime>
#include <cstring>

#include "util.h"
#include "genhand.h"

int inline num_hand(int *a) {
    int h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);
    return NUM_HAND(h);
}

int main() {
    map<int,int> C, ans;
    int h[5] = {0,1,2,3,4};
    int n = 0;
    clock_t start;
    double duration;

    cout << "Calculating..." << endl;
    start = clock();
    gen_two_pairs([&](int *h) { ans[num_hand(h)] = 2; n++; });
    gen_sets([&](int *h) { ans[num_hand(h)] = 2; n++; });
    gen_straights([&](int *h) { ans[num_hand(h)] = 3; n++; });
    gen_flushes([&](int *h) { ans[num_hand(h)] = 4; n++; });
    gen_full_houses([&](int *h) { ans[num_hand(h)] = 8; n++; });
    gen_fours([&](int *h) { ans[num_hand(h)] = 15; n++; });
    gen_straight_flushes([&](int *h) { ans[num_hand(h)] = 30; n++; });
    gen_fives([&](int *h) { ans[num_hand(h)] = 50; n++; });
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << setprecision(5) << duration << "s" << endl;

    cout << n << " iterations to create " << ans.size() << " answers" << endl;
    for(auto p : ans) C[p.second]++;
    for(auto p : C) cout << p.first << "*" << p.second << endl;

    C.clear();

    cout << "Precalculating..." << endl;
    start = clock();
    precalc_win();
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << setprecision(5) << duration << "s" << endl;

    cout << "Calculated counts:" << endl;
    n = 0;
    start = clock();
    do {
        n++;
        int w = win(h), nh = num_hand(h);
        C[w]++;
        if(ans.count(nh) && ans[nh] != w)
            cout << "Wrong answer " << w << " for ", print_hand(h);
        if(w == 15 && ans.count(nh)==0)
            cout << "Missing answer " << w << " for ", print_hand(h);
    } while(next_combi(h, 5, 52));
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << fixed << setprecision(5) << duration << "s" << endl;

    cout << n << " iterations" << endl;
    for(auto p : C) cout << p.first << "*" << p.second << endl;

    return 0;
}

