#include <iostream>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include <string>
#include <cstring>
#include <numeric>

#include "util.h"

using namespace std;

string vals = "23456789TJQKA", suits = "CDHS";

bool input_hand(int *h) {
    string str;

    getline(cin, str);
    if(!str.size()) return false;

    string_hand(h, str);

    return true;
}

void string_hand(int *h, const string &str) {
    for(int i=0; i<5; i++) {
        if(str[i*3]=='?') h[i] = 52;
        else {
            int v, s;
            v = vals.find(toupper(str[i*3]));
            s = suits.find(toupper(str[i*3+1]));
            h[i] = v*4 + s;
        }
    }
}

// Normalize hand to increasing value and suit order
// (suit can increase at most by one within ordered hand)
void norm_hand(int *h) {
    int suit[4]={0,1,2,3};

    // Make bitmap of values within each suit, low value = high bit
    for(int i=0; i<5; i++) if(h[i]!=52) suit[h[i]&3] |= 1 << 15-h[i]/4;
    sort4(suit); // sort suits so highest value suits go first
    int tr[4]; // translation for suits, small values for small suits
    for(int i=0; i<4; i++) tr[suit[i]&3] = 3-i; 
    // We could do faster with (suit[i]&3) ^ (3-i) above and xor below
    for(int i=0; i<5; i++) if(h[i]!=52) h[i] = (h[i]&0xFC) + tr[h[i]&3];
    sort5(h);
}

// Quick and dirty int conversion
int hand_int(int *h) {
    int ret = h[0];
    for(int i=1; i<5; i++) ret = (ret<<6) + h[i];
    return ret;
}

void int_hand(int *h, int hi) {
    for(int i=0; i<5; i++) h[i] = hi >> (20-i*6);
}

int C(int n, int k) {
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for(int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

// Combinatoric numbering of hands
int hand_num(int *a) {
    int sum = 0, h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);
    for(int i=0; i<5; i++) sum += C(h[i], i+1);
    return sum;
}

// Normalized combinatoric numbering -- no functional duplicates
int hand_num_norm(int *a) {
    int sum = 0, h[5];
    memcpy(h, a, 5*sizeof(int));
    norm_hand(h);
    for(int i=0; i<5; i++) sum += C(h[i], i+1);
    return sum;
}

// Combinatoric numbering to a hand
void num_hand(int *h, int n) {
    for(int i=4; i>=0; i--) {
        int p=0;
        while(C(p+1, i+1) <= n) p++;
        h[i] = p;
        n -= C(p, i+1);
    }
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
    if(c[0]+c[1]==4) return 2; //(c[0]*c[1]==4) ? 1 : 2; // Set or two pair

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
