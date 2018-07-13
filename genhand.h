#ifndef __GENHAND_H

#include <functional>

using namespace std;

void gen_hands(function<void(int *)> f, bool joker=true);
void gen_normal_hands(function<void(int *)> f);
void gen_fives(function<void(int *)> f);
void gen_straight_flushes(function<void(int *)> f);
void gen_fours(function<void(int *)> f);
void gen_full_houses(function<void(int *)> f);
void gen_flushes(function<void(int *)> f);
void gen_straights(function<void(int *)> f);
void gen_sets(function<void(int *)> f);
void gen_two_pairs(function<void(int *)> f);
void gen_base_pairs(function<void(int *)> f, bool joker=true);

#endif
