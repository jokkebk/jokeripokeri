#include <iostream>
#include <vector>
#include <map>
#include <bitset>
#include <sstream>
#include <string>
#include <cstring>
#include <numeric>
#include <algorithm>

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
            size_t v, s;
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
    for(int i=0; i<5; i++) if(h[i]!=52) suit[h[i]&3] |= 1 << (15-h[i]/4);
    sort4(suit); // sort suits so highest value suits go first
    int tr[4]; // translation for suits, small values for small suits
    for(int i=0; i<4; i++) tr[suit[i]&3] = 3-i; 
    // We could do faster with (suit[i]&3) ^ (3-i) above and xor below
    for(int i=0; i<5; i++) if(h[i]!=52) h[i] = (h[i]&0xFC) + tr[h[i]&3];
    sort5(h);
}

// Check if hand is normalized, see comments in norm_hand() for details
bool is_normal(int *h) {
   for(int i=1; i<5; i++) if(h[i] < h[i-1]) return false;

   int suit[4]={0,0,0,0};

   for(int i=0; i<5; i++) if(h[i]!=52) suit[h[i]&3] |= 1 << (15-h[i]/4);
   //for(int i=0; i<4; i++) cout << bitset<16>(suit[i]) << endl;
   //cout << endl;
   if(suit[0] < suit[1]) return false;
   if(suit[1] < suit[2]) return false;
   if(suit[2] < suit[3]) return false;
   return true;
}

// Quick and dirty int conversion
int hand_int(int *h) {
    int ret = h[0];
    for(int i=1; i<5; i++) ret = (ret<<6) + h[i];
    return ret;
}

void int_hand(int *h, int hi) {
    for(int i=0; i<5; i++) h[i] = (hi >> (20-i*6)) & 0x3F;
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

// Generate all 150 891 normalized jokeripokeri hands and return as a set
std::set<int> gen_normal_hand_nums() {
	int h[5] = { 0, 1, 2, 3, 4 };
	set<int> uniq;

	do {
		if (SEMINORMAL(h)) uniq.insert(hand_num_norm(h));
	} while (next_combi(h, 5, 52)); 

	return uniq;
}

bool is_paired(int *h) {
    for(int i = 0; i < 4; i++)
      if(h[i] / 4 == h[i + 1] / 4)
          return true;
    return false;
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

	int c[5] = { 1,0,0,0,0 }, n = 0;
	bool flush = (h[0] & 3) == (h[1] & 3) && (h[0] & 3) == (h[2] & 3) &&
			(h[0] & 3) == (h[3] & 3);

    if(h[4]==52) { // joker
		h[0] >>= 2;
		for (int i = 1; i < 4; i++) {
			h[i] >>= 2;
			if (h[i] != h[i - 1]) n++;
			c[n]++;
		}

		if(c[0] == 4) return 50; // five of a kind
		if(c[0] == 3 || c[1] == 3) return 15; // four of a kind
		if(c[2] == 0) return 8; // full house (has to be 2+2)
        if(c[0] == 2 || c[1] == 2 || c[2] == 2) return 2;

		if(h[3] - h[0] <= 4 || (h[2] <= 3 && h[3] == 12))
			return flush ? 30 : 3; // straight (flush)
    } else {
		flush = flush && (h[0] & 3) == (h[4] & 3);

		h[0] >>= 2;
		for (int i = 1; i < 5; i++) {
			h[i] >>= 2;
			if (h[i] != h[i - 1]) n++;
			c[n]++;
		}

        if(c[2]==0) return (c[0]==4 || c[1]==4) ? 15 : 8;
        if(c[3]==0) return 2;

		if (n == 4 && (h[4] - h[0] == 4 || (h[3] == 3 && h[4] == 12)))
			return flush ? 30 : 3; // Straight (flush)

	}

	return flush ? 4 : 0;
}

// Win with mixed cards, no flush
int win_mixed(int *a) {
    int h[5];
    memcpy(h, a, 5*sizeof(int));
    sort5(h);

	int c[5] = { 1,0,0,0,0 }, n = 0;

    if(h[4]==52) { // joker
		h[0] >>= 2;
		for (int i = 1; i < 4; i++) {
			h[i] >>= 2;
			if (h[i] != h[i - 1]) n++;
			c[n]++;
		}

		if(c[0] == 4) return 50; // five of a kind
		if(c[0] == 3 || c[1] == 3) return 15; // four of a kind
		if(c[2] == 0) return 8; // full house (has to be 2+2)
        if(c[0] == 2 || c[1] == 2 || c[2] == 2) return 2;

		if(h[3] - h[0] <= 4 || (h[2] <= 3 && h[3] == 12))
			return 3; // straight (no flush)
    } else {
		h[0] >>= 2;
		for (int i = 1; i < 5; i++) {
			h[i] >>= 2;
			if (h[i] != h[i - 1]) n++;
			c[n]++;
		}

        if(c[2]==0) return (c[0]==4 || c[1]==4) ? 15 : 8;
        if(c[3]==0) return 2;

		if (n == 4 && (h[4] - h[0] == 4 || (h[3] == 3 && h[4] == 12)))
			return 3; // Straight (no flush)

	}

	return 0;
}
