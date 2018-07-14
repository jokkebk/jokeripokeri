#pragma once

#include <string>
#include <ctime>
#include <set>
#include <map>
#include <unordered_map>

#define SWAPIF(a,b) if(a>b) { a ^= b; b ^= a; a ^= b; }
#define SEMINORMAL(h) (((h)[0] & 3)==0 && (((h)[1]&3)<2) && (((h)[2]&3)<3))
#define ONE_BIT(v) (!((v) & ((v)-1)))

inline int countOnes5b(int s) { // it's fast!
    s = (s&(1+4+16)) + ((s&(2+8))>>1);
    return (s>>4) + ((s>>2)&3) + (s&3);
}

inline void sort5(int *a) {
    SWAPIF(a[1], a[2]);
    SWAPIF(a[3], a[4]);
    SWAPIF(a[1], a[3]);
    SWAPIF(a[0], a[2]);
    SWAPIF(a[2], a[4]);
    SWAPIF(a[0], a[3]);
    SWAPIF(a[0], a[1]);
    SWAPIF(a[2], a[3]);
    SWAPIF(a[1], a[2]);
}
 
inline void sort4(int *a) {
    SWAPIF(a[0], a[1]);
    SWAPIF(a[2], a[3]);
    SWAPIF(a[1], a[3]);
    SWAPIF(a[0], a[2]);
    SWAPIF(a[1], a[2]);
}

inline size_t myrand() {
    time_t res = std::time(nullptr);
    std::hash<std::string> hasher;
    std::string s(std::asctime(std::localtime(&res)));
    return hasher(s);
}

#define NUM_HAND(h) ((h[4]<<24)+(h[3]<<18)+(h[2]<<12)+(h[1]<<6)+h[0])

bool input_hand(int *h);
void string_hand(int *h, const std::string &str);
std::string hand_string(int *h, int n=5, int highlight=0);
void print_hand(int *h, int n=5, int highlight=0);

void norm_hand(int *h);
bool is_normal(int *h);
int hand_int(int *h);
void int_hand(int *h, int hi);
int hand_num(int *a);
int hand_num_norm(int *a);
void num_hand(int *h, int n);
bool is_paired(int *h);

int C(int n, int k);

int win(int *a);
int win_mixed(int *a);

bool next_combi(int *c, int n, int max);

std::map<int,int> count_wins(int *h, int s);
