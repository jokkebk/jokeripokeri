import sys
from random import randint
from collections import Counter
from itertools import product, combinations
from util import *

# Match hands to patterns and return matching selection or 0 if no match
# Following patterns are supported:
#
# Pair-type patterns:
# uuuu and uuu? = fours (u is the most_common value)
# uuuvv and uuvv? = full house (v is the 2nd most_common value)
#
# Flush-type patterns:
# sssss and ssss? = flush (s is the most common suit)
# ssss and sss? = flush draw
#
# Straight-type patterns:
# 
# 23456789T abcde = all straights except A2345
# 2 abcdm = the A2345 straight
# 3456789T abc s? = open-ended straight flush draw with joker
#
# Joker goes always last.
def match_hand(h, p):
    jmask = (1<<h.index(52)) if 52 in h else 0
    if not jmask and p[-1] == '?': return 0 # no required joker

    if p[0] == 'u': # Pair-type pattern
        vc = Counter(c>>2 for c in h).most_common(2)
        if p.count('u') != vc[0][1]: return 0
        v = p.count('v')
        if v == 0: return jmask + val_mask(h, vc[0][0])
        elif p.count('v') == vc[1][1]:
            return jmask + val_mask(h, vc[0][0]) + val_mask(h, vc[1][0])
    elif p[0] == 's': # Flush-type pattern
        suit, sc = Counter(c&3 for c in h if c != 52).most_common(1)[0]
        if jmask: sc += 1 # Count joker in all suits
        if p.count('s') == sc:
            return sum((1<<i) for i in range(5) \
                    if (h[i]&3)==suit or h[i]==52)
    else: # Straight-type pattern
        val_pat = p.split()
        hmasks = [] # Do it with arrays to support suited
        if len(val_pat)>2 and val_pat[2][0]=='s': # Suited
            suited = True
            for s in set(c&3 for c in h):
                hmask = sum(1<<c//4 for c in h if (c&3)==s and c!=52)
                if bin(hmask).count('1') >= len(val_pat[1]):
                    hmasks.append((hmask, s))
        else:
            hmasks.append((sum(1<<c//4 for c in h), 0))

        mask = sum(1<<ord(c)-ord('a') for c in val_pat[1])

        match = False
        for hmask, s in hmasks:
            for v in val_pat[0]:
                v = valS.index(v)
                if ((hmask>>v) & mask) != mask: continue
                if suited:
                    return sum(card_mask(h, (ord(c)-ord('a')+v)*4+s) \
                            for c in val_pat[1]) + jmask
                else:
                    return sum(val_mask(h, ord(c)-ord('a')+v) \
                            for c in val_pat[1]) + jmask

    return 0

if __name__ == "__main__":
    pat = [
    #'uuuu?', # fives
    #'uuuu', 'uuu?', # fours
    #'uuuvv', 'uuvv?', # full house
    #'uuu', 'uu?', 'uuvv', # set or two pair
    '3456789T abc s?', # super straight flush draw
    ]

    for h in combinations(range(0,53), 5):
        for p in pat:
            sel = match_hand(h, p)
            if sel:
                print(hand_str(h, sel))
                break
