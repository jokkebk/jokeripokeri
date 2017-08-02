from collections import Counter
from itertools import product
from util import num_hand, val_mask, hand_str, C

deckS = [''.join(p) for p in product('23456789TJQKA', 'CDHS')]+['??']

# Match hands to patterns and return matching selection or 0 if no match
# Following patterns are supported:
#
# Pair-type patterns:
# uuuu uuu? = fours (u is the most_common value)
# uuuvv uuvv? = full house (v is the 2nd most_common value)
#
# Flush-type patterns:
# sssss ssss? = flush (s is the most common suit)
# ssss sss? = flush draw
#
# Straight-type patterns:
# 
#
# Joker goes always last. You cannot mix different types of patterns.
def hand_match(h, pat):
    # First calculate some data depending on pattern type
    joker = 52 in h
    if pat[0] == 'u': # Pair-type pattern
        vc = Counter(c>>2 for c in h).most_common(2)
        jmask = (1<<h.index(52)) if joker else 0
    elif pat[0] == 's': # Flush-type pattern
        suit, sc = Counter(c&3 for c in h if c != 52).most_common(1)[0]
        if joker: sc += 1 # Count joker in all suits

    # Then loop through the patterns
    for p in pat.split():
        if joker and p[-1] != 52: continue # no required joker

        if p[0] == 'u': # Pair-type pattern
            if p.count('u') != vc[0][1]: continue
            v = p.count('v')
            if v == 0: return jmask + val_mask(h, vc[0][0])
            elif p.count('v') == vc[1][1]:
                return jmask + val_mask(h, vc[0][0]) + val_mask(h, vc[1][0])
        elif p[0] == 's': # Flush-type pattern
            if p.count('s') == sc:
                return sum((1<<i) for i in range(5) if (h[i]&3)==suit or h[i]==52)

    return 0

pat = [
'uuuu?', # fives
'uuuu uuu?', # fours
'uuuvv uuvv?', # full house
'uuu uu? uuvv', # set or two pair
]
cnt = [0] * len(pat)

for num in range(0, C(53, 5), 1):
    h = num_hand(num)
    for i,p in enumerate(pat):
        if hand_match(h, p):
            cnt[i] += 1
            break
    #sel = hand_match(h, 'sss')
    #if sel: print(hand_str(h, sel), 'sss')

print(cnt)

    #hand = [deckS.index(s.upper()) for s in input().strip().split()]
    #if not hand: break
#h = [0, 1, 5, 10, 52]
#print(hand_str(h), hand_match(h, 'sss'))
