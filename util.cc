#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <cstring>
#include <numeric>

#include "util.h"

using namespace std;

string vals = "23456789TJQKA", suits = "HDCS";

bool input_hand(int *h) {
    string str;

    getline(cin, str);
    if(!str.size()) return false;

    for(int i=0; i<5; i++) {
        if(str[i*3]=='?') h[i] = 52;
        else {
            int v, s;
            v = vals.find(toupper(str[i*3]));
            s = suits.find(toupper(str[i*3+1]));
            h[i] = v*4 + s;
        }
    }

    return true;
}

string hand_string(int *h, int n, int highlight) {
    ostringstream ss;
    for(int i=0; i<n; i++) {
        if((highlight>>i)&1) ss << "[";
        if(h[i] == 52) ss << "??";
        else ss << vals[h[i]>>2] << suits[h[i]&3];
        if((highlight>>i)&1) ss << "]";
        if(i+1<n) ss << " ";
    }
    return ss.str();
}

void print_hand(int *h, int n, int highlight) {
    cout << hand_string(h, n, highlight) << endl;
}

// Generate combinations
bool next_combi(int *c, int n, int max) {
    int i;
    for(i=n-1; i>=0; i--) {
        c[i]++;
        if(c[i] <= max-n+i+1) break;
    }
    if(i<0) return false;
    for(i++; i<n; i++) c[i] = c[i-1]+1;
    return true;
}

int win(int *a) {
    int h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);

    if(h[4]==52) { // Joker
        int best = 0;
        for(h[4]=h[0]&3; h[4]<52; h[4]+=4) best = max(best, win(h));
        return best;
    }

    int c[5] = {1,0,0,0,0}, n=0;
    bool flush = (h[0]&3)==(h[1]&3) && (h[0]&3)==(h[2]&3) &&
        (h[0]&3)==(h[3]&3) && (h[0]&3)==(h[4]&3);

    h[0]>>=2;
    for(int i=1; i<5; i++) {
        h[i]>>=2;
        if(h[i] != h[i-1]) n++;
        c[n]++;
    }

    if(c[0]==5) return 50; // Five of a kind
    SWAPIF(c[1], c[0]); // note: reverse
    if(c[0]==4) return 15; // Four of a kind
    SWAPIF(c[2], c[1]); // note: reverse
    if(c[0]+c[1]==5) return 8; // Full house
    if(c[0]+c[1]==4) return 2; // Set or two pair

    if(n==4 && (h[4]-h[0]==4 || (h[3]==3 && h[4]==12)))
        return flush ? 30 : 3; // Straight (flush)

    return flush ? 4 : 0;
}

map<int,int> precalc;

void precalc_win() {
    int h[5]={0,1,2,3,4};
    do {
        precalc[NUM_HAND(h)] = win(h);
    } while(next_combi(h, 5, 52));
}

int quickwin(int *a) {
    int h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);
    return precalc[NUM_HAND(h)];
}
